/**
 * @file datalink_serial.hpp
 * Public interface for comms protocol datalink for serial ports.
 *
 * @authors Graham Bartlett
 * @copyright Prior Scientific Instruments Ltd., 2021
 */

#include "pch.h"

#ifndef DATALINK_SERIAL_HPP
#define DATALINK_SERIAL_HPP

#include <string>

#ifdef WIN32

#include "windows.h"
#include "winbase.h"

#else

#include <termios.h>
#include <unistd.h>

#endif

 // #######################################################

 /** Class to handle serial comms.
  */
class DatalinkSerial {
public:
	/** Constructor.
	*/
	DatalinkSerial();

	/** Destructor.
	*/
	~DatalinkSerial();

	/** Open serial comms.
	* @param port Port name
	* @returns True if opened successfully
	*/
	bool Open(const char* port);

	/** Close serial comms.
	*/
	void Close();

	/** Read from the serial port.

	 * @param readBuffer Buffer in which bytes read from datalink layer will be stored
	 * @param readBufferLength Length of readBuffer in bytes
	 * @param minimumReadLength Minimum number of bytes to read from datalink layer before unblocking.  If zero, do not block and simply return whatever is in buffer.  This takes precedence over readTimeoutMs.
	 * @param readTimeoutMs Timeout in ms when reading datalink layer.  If zero, block indefinitely until readBufferLength has been read.
	 * @param handle Pointer to DatalinkSerial instance
	 * @param unused Unused context value
	 * @returns Number of bytes read (zero if no bytes read in readTimeoutMs time), or negative if there is an error (-1=invalid handle requiring link restart, other values for other errors)
	 */
	static int Read(
		char* readBuffer,
		const unsigned int readBufferLength,
		const unsigned int minimumReadLength,
		const unsigned int readTimeoutMs,
		void* handle,
		int unused);

	/** Write to the serial port.
	 * @param writeBuffer Buffer in which bytes to write to datalink layer are stored
	 * @param writeBufferLength Number of bytes to write to datalink layer
	 * @param handle Pointer to DatalinkSerial instance
	 * @param unused Unused context value
	 * @returns Zero if successful, or negative if there is an error
	 */
	static int Write(
		const char* writeBuffer,
		const unsigned int writeBufferLength,
		void* handle,
		int unused);

private:
	/** Prevent use of copy constructor.
	*/
	DatalinkSerial(DatalinkSerial&);

	/** Name of UART device */
	std::string device;

#ifdef WIN32

	/** File stream handle for UART */
	HANDLE filestream;

	/** Timeouts */
	COMMTIMEOUTS timeouts;

#else

	/** File stream handle for UART */
	int filestream;

	/** UART configuration structure */
	struct termios config;

#endif
};

#endif