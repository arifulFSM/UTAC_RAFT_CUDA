/**
 * @file datalink_serial.cpp
 * Public interface for comms protocol datalink for serial ports on Windows.
 *
 * @authors Graham Bartlett
 * @copyright Prior Scientific Instruments Ltd., 2021
 */

#include "pch.h"
#include "datalink_serial.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

 // #######################################################

DatalinkSerial::DatalinkSerial() :
	device(),
	filestream(INVALID_HANDLE_VALUE) {}

DatalinkSerial::DatalinkSerial(DatalinkSerial&) :
	DatalinkSerial() {
	/* Should never be called */
}

DatalinkSerial::~DatalinkSerial() {
	Close();
}

bool DatalinkSerial::Open(const char* port) {
	DCB dcbBlock = { 0 };
	bool isOk = true;

	if (filestream != INVALID_HANDLE_VALUE) {
		/* Init already called, so leave it open */
		isOk = false;
	}
	else {
		/* Copy port name */
		device.assign(port);

		/* Open/reopen serial device if the filestream is not open.  Note that this
		 * selects non-overlapped reads/writes, i.e. all reads and writes must
		 * complete before the read/write function returns.  This is what we want
		 * for reading; we don't need it for writing, but the consequences are not
		 * a problem here.
		 */
		filestream = CreateFileA(device.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			0,
			0);
		if (filestream == INVALID_HANDLE_VALUE) {
			/* Failed opening serial device */
			isOk = false;
		}
		else {
			/* Get default DCB structure, and bail out if this fails */
			if (!GetCommState(filestream, &dcbBlock)) {
				isOk = false;
			}
			else {
				/* Set up our own values in the DCB structure */
				dcbBlock.BaudRate = 115200;
				dcbBlock.fBinary = TRUE;
				dcbBlock.fParity = FALSE;
				dcbBlock.fOutxCtsFlow = FALSE;
				dcbBlock.fOutxDsrFlow = FALSE;
				// Left permanently enabled, which keeps sync alive
				dcbBlock.fDtrControl = DTR_CONTROL_ENABLE;
				dcbBlock.fDsrSensitivity = FALSE;
				// No checks on Rx buffer
				dcbBlock.fTXContinueOnXoff = TRUE;
				dcbBlock.fOutX = FALSE;
				dcbBlock.fInX = FALSE;
				dcbBlock.fErrorChar = FALSE;
				dcbBlock.fNull = FALSE;
				// Left permanently enabled, which keeps sync alive
				dcbBlock.fRtsControl = RTS_CONTROL_ENABLE;
				dcbBlock.fAbortOnError = TRUE;
				dcbBlock.ByteSize = 8;
				dcbBlock.Parity = NOPARITY;
				dcbBlock.StopBits = ONESTOPBIT;

				/* Now set up port */
				if (!SetCommState(filestream, &dcbBlock)) {
					isOk = false;
				}
			}

			/* Get default timeouts, and bail out if this fails */
			if (!GetCommTimeouts(filestream, &(timeouts))) {
				isOk = false;
			}
			else {
				/* Set up our timeout, all in ms.
				 * Reading just has a 10ms timeout on overall read.
				 * Writing has no timeout.
				 */
				timeouts.ReadIntervalTimeout = MAXDWORD;
				timeouts.ReadTotalTimeoutMultiplier = 0;
				timeouts.ReadTotalTimeoutConstant = 10;
				timeouts.WriteTotalTimeoutMultiplier = 0;
				timeouts.WriteTotalTimeoutConstant = 0;

				/* Now set timeouts */
				if (!SetCommTimeouts(filestream, &(timeouts))) {
					isOk = false;
				}
			}

			if (!isOk) {
				/* Failed to set up port, so close it */
				CloseHandle(filestream);
				filestream = INVALID_HANDLE_VALUE;
			}
		}
	}

	return isOk;
}

void DatalinkSerial::Close() {
	device.clear();

	if (filestream != INVALID_HANDLE_VALUE) {
		CloseHandle(filestream);
	}

	filestream = INVALID_HANDLE_VALUE;
}

int DatalinkSerial::Read(char* readBuffer,
	const unsigned int readBufferLength,
	const unsigned int minimumReadLength,
	const unsigned int readTimeoutMs,
	void* handle,
	int unused) {
	(void)unused;
	DatalinkSerial* instance = (DatalinkSerial*)handle;
	int bytesRead;
	BOOL readStatus;
	DWORD readFileBytesRead, waitStatus;
	OVERLAPPED overlappedStatus = { 0 };
	DWORD errorCode;

	/* This parameter is not used in the Win32 implementation */
	(void)readBufferLength;

	/* Set up timeout for reading */
	instance->timeouts.ReadTotalTimeoutConstant = readTimeoutMs;
	SetCommTimeouts(instance->filestream, &instance->timeouts);

	/* Read data */
	readStatus = ReadFile(instance->filestream,
		readBuffer,
		minimumReadLength,
		&readFileBytesRead,
		&overlappedStatus);
	errorCode = GetLastError();
	switch (errorCode) {
	case ERROR_SUCCESS:
		/* Read OK */
		bytesRead = readFileBytesRead;
		break;

	case ERROR_IO_PENDING:
		/* Only partial read, so wait for completion */
		waitStatus = WaitForSingleObject(overlappedStatus.hEvent, readTimeoutMs);
		switch (waitStatus) {
		case WAIT_OBJECT_0:
			/* Completed */
			readStatus = GetOverlappedResult(instance->filestream,
				&overlappedStatus,
				&readFileBytesRead,
				FALSE);
			if (!readStatus) {
				/* Error reading */
				bytesRead = -2;
			}
			else {
				bytesRead = readFileBytesRead;
			}
			break;

		case WAIT_TIMEOUT:
			/* Timeout waiting for read */
			bytesRead = 0;
			break;

		default:
			/* Some other error during overlapped period of read */
			bytesRead = -4;
			break;
		}
		break;

	case ERROR_INVALID_HANDLE:
		/* Handle is not valid for Rx, so need to close and reopen link */
		bytesRead = -1;
		break;

	default:
		/* Some error in setting up read */
		bytesRead = -5;
		break;
	}

	return bytesRead;
}

int DatalinkSerial::Write(const char* writeBuffer,
	const unsigned int writeBufferLength,
	void* handle,
	int unused) {
	(void)unused;
	DatalinkSerial* instance = (DatalinkSerial*)handle;
	DWORD dummyBytesWritten;
	BOOL status;
	int returnValue = 0;

	/* Write data to UART */
	status = WriteFile(instance->filestream,
		writeBuffer,
		writeBufferLength,
		&dummyBytesWritten,
		NULL);

	if (!status) {
		/* Error writing */
		returnValue = -1;
	}

	return returnValue;
}