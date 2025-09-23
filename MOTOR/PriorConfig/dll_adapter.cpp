/**
@file dll_adapter.cpp
Defines platform-independent adapter around command interface DLL.

@authors Graham Bartlett
@copyright Prior Scientific Instruments Ltd., 2016
*/

#include "pch.h"
#include "dll_adapter.hpp"

#include <string>
#include <iostream>

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
/* Windows */
#include <Windows.h>

#else
/* Linux/Posix */
#include <dlfcn.h>

#endif

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
#include "tinythread.h"
#endif

DllAdapter::DllAdapter() :
	dllHandle(NULL),
	instanceHandle(NULL),
	sessionIsOpen(false),
	initFn(NULL),
	findDevicesFn(NULL),
	getDeviceFn(NULL),
	openSessionFn(NULL),
	openSessionDatalinkFn(NULL),
	closeSessionFn(NULL),
	uninitFn(NULL),
	getChannelsFn(NULL),
	findCommandsFn(NULL),
	getCommandFn(NULL),
	getCommandDescriptionFn(NULL),
	getCommandParametersFn(NULL),
	getCommandParameterNameFn(NULL),
	getCommandParameterUnitsTypeFn(NULL),
	getCommandParameterUnitsFn(NULL),
	getCommandResultsFn(NULL),
	getCommandResultNameFn(NULL),
	getCommandResultUnitsTypeFn(NULL),
	getCommandResultUnitsFn(NULL),
	getEnumerationsFn(NULL),
	getEnumerationNameFn(NULL),
	getEnumerationValuesFn(NULL),
	getEnumerationValueFn(NULL),
	doCommandFn(NULL),
	getResultNameFn(NULL),
	getResultFn(NULL),
	getDllVersionFn(NULL) {}

DllAdapter::DllAdapter(const DllAdapter& copy) {
	/* Should never be executed */
}

DllAdapter::~DllAdapter() {
#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if ((instanceHandle != NULL) && (uninitFn != NULL)) {
		uninitFn(instanceHandle);
	}

	instanceHandle = NULL;

	FreeDllInterface();

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif
}

DllAdapterStatus DllAdapter::Init(const std::string& dllPath) {
	DllAdapterStatus isOk;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	isOk = InitDllInterface(dllPath);

	if ((isOk == DLL_ADAPTER_STATUS_SUCCESS) && (initFn != NULL)) {
		/* Loaded DLL OK */
		instanceHandle = initFn();
		if (instanceHandle == NULL) {
			isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

DllAdapterStatus DllAdapter::GetDevices(std::list<std::string>& devices) {
	DllAdapterStatus isOk = DLL_ADAPTER_STATUS_SUCCESS;
	int numDevices;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if ((instanceHandle == NULL) ||
		(findDevicesFn == NULL) ||
		(getDeviceFn == NULL)) {
		/* Not set up yet */
		isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
	}
	else {
		devices.clear();

		numDevices = findDevicesFn(instanceHandle);
		if (numDevices < 0) {
			/* Error */
			isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL - numDevices);
		}
		else {
			for (int i = 0; (i < numDevices) && (isOk == DLL_ADAPTER_STATUS_SUCCESS); i++) {
				int length = getDeviceFn(instanceHandle, i, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 1000 - length);
				}

				devices.push_back("");
				std::string& backEntry = devices.back();
				backEntry.resize(length);

				length = getDeviceFn(instanceHandle, i, const_cast<char*>(backEntry.c_str()), length);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 2000 - length);
				}
				else {
					/* Strip null terminator */
					if (*backEntry.rbegin() == '\0') {
						backEntry.resize(length - 1);
					}
				}
			}
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

DllAdapterStatus DllAdapter::OpenSession(const std::string& device) {
	DllAdapterStatus isOk = DLL_ADAPTER_STATUS_SUCCESS;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if ((instanceHandle == NULL) || (openSessionFn == NULL)) {
		/* Not set up yet */
		isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
	}
	else {
		int result = openSessionFn(instanceHandle, device.c_str());
		if (result == 1) {
			sessionIsOpen = true;
			isOk = DLL_ADAPTER_STATUS_SUCCESS;
		}
		else {
			isOk = DLL_ADAPTER_STATUS_ERROR_PORT_NOT_AVAILABLE;
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

DllAdapterStatus DllAdapter::OpenSessionDatalink(ControllerInterfaceDatalinkReadFn datalinkRead,
	ControllerInterfaceDatalinkWriteFn datalinkWrite,
	void* contextPtr,
	int contextVal) {
	DllAdapterStatus isOk = DLL_ADAPTER_STATUS_SUCCESS;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if (instanceHandle == NULL) {
		/* Not set up yet */
		isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
	}
	else if (openSessionDatalinkFn == NULL) {
		/* Older version of DLL */
		isOk = DLL_ADAPTER_STATUS_ERROR_DLL_INVALID;
	}
	else {
		int result = openSessionDatalinkFn(instanceHandle, datalinkRead, datalinkWrite, contextPtr, contextVal);
		if (result == 1) {
			sessionIsOpen = true;
			isOk = DLL_ADAPTER_STATUS_SUCCESS;
		}
		else {
			isOk = DLL_ADAPTER_STATUS_ERROR_PORT_NOT_AVAILABLE;
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

void DllAdapter::CloseSession() {
#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if ((instanceHandle == NULL) || (closeSessionFn == NULL)) {
		/* Not set up yet */
	}
	else {
		closeSessionFn(instanceHandle);
	}

	sessionIsOpen = false;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif
}

bool DllAdapter::IsSessionOpen() {
	return sessionIsOpen;
}

int DllAdapter::GetChannels() {
	int channels = 0;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if ((instanceHandle == NULL) || (getChannelsFn == NULL)) {
		/* Not set up yet */
	}
	else {
		channels = getChannelsFn(instanceHandle);
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return channels;
}

DllAdapterStatus DllAdapter::FindCommands(std::list<std::string>& commands, const std::string& filter) {
	DllAdapterStatus isOk = DLL_ADAPTER_STATUS_SUCCESS;
	int numCommands;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if ((instanceHandle == NULL) ||
		(findCommandsFn == NULL) ||
		(getCommandFn == NULL)) {
		/* Not set up yet */
		isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
	}
	else {
		commands.clear();

		numCommands = findCommandsFn(instanceHandle, filter.c_str());
		if (numCommands < 0) {
			/* Error */
			isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL - numCommands);
		}
		else {
			for (int i = 0; (i < numCommands) && (isOk == DLL_ADAPTER_STATUS_SUCCESS); i++) {
				int length = getCommandFn(instanceHandle, i, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 1000 - length);
				}
				else {
					commands.push_back("");
					std::string& backEntry = commands.back();
					backEntry.resize(length);

					length = getCommandFn(instanceHandle, i, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 2000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}
			}

			/* DLL does not filter command list, so we must do it here */
			if ((isOk == DLL_ADAPTER_STATUS_SUCCESS) && (filter.size() > 0)) {
				std::list<std::string>::iterator ptr, toDelete;
				ptr = commands.begin();
				while (ptr != commands.end()) {
					if (ptr->compare(0, filter.size(), filter) != 0) {
						/* Delete unwanted commands from list */
						toDelete = ptr;
						ptr++;
						commands.erase(toDelete);
					}
					else {
						/* Keep */
						ptr++;
					}
				}
			}
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

DllAdapterStatus DllAdapter::GetCommandDescription(const std::string& commandName, std::string& description) {
	DllAdapterStatus isOk = DLL_ADAPTER_STATUS_SUCCESS;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if ((instanceHandle == NULL) ||
		(getCommandDescriptionFn == NULL)) {
		/* Not set up yet */
		isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
	}
	else {
		int length = getCommandDescriptionFn(instanceHandle, commandName.c_str(), NULL, 0);
		if (length <= 0) {
			/* Error */
			isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL - length);;
		}
		else {
			description.resize(length);

			length = getCommandDescriptionFn(instanceHandle, commandName.c_str(), const_cast<char*>(description.c_str()), length);
			if (length <= 0) {
				/* Error */
				isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 1000 - length);
			}
			else {
				/* Strip null terminator */
				if (*description.rbegin() == '\0') {
					description.resize(length - 1);
				}
			}
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

DllAdapterStatus DllAdapter::GetCommandParameters(const std::string& commandName, const int channel, std::list<std::string>& commandParameterNames, std::list<std::string>& commandParameterUnitTypes, std::list<std::string>& commandParameterUnits) {
	DllAdapterStatus isOk = DLL_ADAPTER_STATUS_SUCCESS;
	int numParameters;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if ((instanceHandle == NULL) ||
		(getCommandParametersFn == NULL) ||
		(getCommandParameterNameFn == NULL) ||
		(getCommandParameterUnitsTypeFn == NULL) ||
		(getCommandParameterUnitsFn == NULL)) {
		/* Not set up yet */
		isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
	}
	else {
		commandParameterNames.clear();
		commandParameterUnitTypes.clear();
		commandParameterUnits.clear();

		numParameters = getCommandParametersFn(instanceHandle, commandName.c_str());
		if (numParameters < 0) {
			/* Error */
			isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL - numParameters);
		}
		else {
			for (int i = 0; (i < numParameters) && (isOk == DLL_ADAPTER_STATUS_SUCCESS); i++) {
				int length = getCommandParameterNameFn(instanceHandle, commandName.c_str(), i, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 1000 - length);
				}
				else {
					commandParameterNames.push_back("");
					std::string& backEntry = commandParameterNames.back();
					backEntry.resize(length);

					length = getCommandParameterNameFn(instanceHandle, commandName.c_str(), i, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 2000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}

				length = getCommandParameterUnitsTypeFn(instanceHandle, commandName.c_str(), i, channel, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 3000 - length);
				}
				else {
					commandParameterUnitTypes.push_back("");
					std::string& backEntry = commandParameterUnitTypes.back();
					backEntry.resize(length);

					length = getCommandParameterUnitsTypeFn(instanceHandle, commandName.c_str(), i, channel, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 4000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}

				length = getCommandParameterUnitsFn(instanceHandle, commandName.c_str(), i, channel, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 5000 - length);
				}
				else {
					commandParameterUnits.push_back("");
					std::string& backEntry = commandParameterUnits.back();
					backEntry.resize(length);

					length = getCommandParameterUnitsFn(instanceHandle, commandName.c_str(), i, channel, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 6000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}
			}
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

DllAdapterStatus DllAdapter::GetCommandResults(const std::string& commandName, const int channel, std::list<std::string>& commandResultNames, std::list<std::string>& commandResultUnitTypes, std::list<std::string>& commandResultUnits) {
	DllAdapterStatus isOk = DLL_ADAPTER_STATUS_SUCCESS;
	int numResults;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if ((instanceHandle == NULL) ||
		(getCommandResultsFn == NULL) ||
		(getCommandResultNameFn == NULL) ||
		(getCommandParameterUnitsTypeFn == NULL) ||
		(getCommandParameterUnitsFn == NULL)) {
		/* Not set up yet */
		isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
	}
	else {
		commandResultNames.clear();
		commandResultUnitTypes.clear();
		commandResultUnits.clear();

		numResults = getCommandResultsFn(instanceHandle, commandName.c_str());
		if (numResults < 0) {
			/* Error */
			isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL - numResults);
		}
		else {
			for (int i = 0; (i < numResults) && (isOk == DLL_ADAPTER_STATUS_SUCCESS); i++) {
				int length;
				length = getCommandResultNameFn(instanceHandle, commandName.c_str(), i, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 1000 - length);
				}
				else {
					commandResultNames.push_back("");
					std::string& backEntry = commandResultNames.back();
					backEntry.resize(length);

					length = getCommandResultNameFn(instanceHandle, commandName.c_str(), i, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 2000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}

				length = getCommandResultUnitsTypeFn(instanceHandle, commandName.c_str(), i, channel, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 3000 - length);
				}
				else {
					commandResultUnitTypes.push_back("");
					std::string& backEntry = commandResultUnitTypes.back();
					backEntry.resize(length);

					length = getCommandResultUnitsTypeFn(instanceHandle, commandName.c_str(), i, channel, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 4000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}

				length = getCommandResultUnitsFn(instanceHandle, commandName.c_str(), i, channel, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 5000 - length);
				}
				else {
					commandResultUnits.push_back("");
					std::string& backEntry = commandResultUnits.back();
					backEntry.resize(length);

					length = getCommandResultUnitsFn(instanceHandle, commandName.c_str(), i, channel, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 6000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}
			}
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

DllAdapterStatus DllAdapter::GetEnumerationNames(std::list<std::string>& enumerations) {
	DllAdapterStatus isOk = DLL_ADAPTER_STATUS_SUCCESS;
	int numEnums;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if (instanceHandle == NULL) {
		/* Not set up yet */
		isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
	}
	else if ((getEnumerationsFn == NULL) || (getEnumerationNameFn == NULL)) {
		/* Older version of DLL */
		return DLL_ADAPTER_STATUS_ERROR_DLL_INVALID;
	}
	else {
		enumerations.clear();

		numEnums = getEnumerationsFn(instanceHandle);
		if (numEnums < 0) {
			/* Error */
			isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL - numEnums);
		}
		else {
			for (int i = 0; (i < numEnums) && (isOk == DLL_ADAPTER_STATUS_SUCCESS); i++) {
				int length = getEnumerationNameFn(instanceHandle, i, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 7000 - length);
				}
				else {
					enumerations.push_back("");
					std::string& backEntry = enumerations.back();
					backEntry.resize(length);

					length = getEnumerationNameFn(instanceHandle, i, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 8000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}
			}
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

DllAdapterStatus DllAdapter::GetEnumerationValues(const std::string& enumerationName, std::list<std::string>& enumerationValues) {
	DllAdapterStatus isOk = DLL_ADAPTER_STATUS_SUCCESS;
	int numEnums;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if (instanceHandle == NULL) {
		/* Not set up yet */
		isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
	}
	else if ((getEnumerationValuesFn == NULL) || (getEnumerationValueFn == NULL)) {
		/* Older version of DLL */
		return DLL_ADAPTER_STATUS_ERROR_DLL_INVALID;
	}
	else {
		enumerationValues.clear();

		numEnums = getEnumerationValuesFn(instanceHandle, enumerationName.c_str());
		if (numEnums < 0) {
			/* Error */
			isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL - numEnums);
		}
		else {
			for (int i = 0; (i < numEnums) && (isOk == DLL_ADAPTER_STATUS_SUCCESS); i++) {
				int length = getEnumerationValueFn(instanceHandle, enumerationName.c_str(), i, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 7000 - length);
				}
				else {
					enumerationValues.push_back("");
					std::string& backEntry = enumerationValues.back();
					backEntry.resize(length);

					length = getEnumerationValueFn(instanceHandle, enumerationName.c_str(), i, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 8000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}
			}
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

DllAdapterStatus DllAdapter::DoCommand(const std::string& command, std::list<std::string>& resultNames, std::list<std::string>& resultValues) {
	DllAdapterStatus isOk = DLL_ADAPTER_STATUS_SUCCESS;
	int numResults;

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.lock();
#endif

	if ((instanceHandle == NULL) ||
		(doCommandFn == NULL) ||
		(getResultNameFn == NULL) ||
		(getResultFn == NULL)) {
		/* Not set up yet */
		isOk = DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP;
	}
	else {
		resultNames.clear();
		resultValues.clear();

		numResults = doCommandFn(instanceHandle, command.c_str());
		if (numResults < 0) {
			/* Error */
			if (numResults == -11) {
				isOk = DLL_ADAPTER_STATUS_ERROR_UNKNOWN_COMMAND;
			}
			else if (numResults == -4) {
				isOk = DLL_ADAPTER_STATUS_ERROR_INCORRECT_PARAMS;
			}
			else {
				/* Other error */
				isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_COMMS_ERROR - numResults);
			}
		}
		else {
			for (int i = 0; (i < numResults) && (isOk == DLL_ADAPTER_STATUS_SUCCESS); i++) {
				int length;
				length = getResultNameFn(instanceHandle, i, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL - length);
				}
				else {
					resultNames.push_back("");
					std::string& backEntry = resultNames.back();
					backEntry.resize(length);

					length = getResultNameFn(instanceHandle, i, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 1000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}

				length = getResultFn(instanceHandle, i, NULL, 0);
				if (length <= 0) {
					/* Error */
					isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 2000 - length);
				}
				else {
					resultValues.push_back("");
					std::string& backEntry = resultValues.back();
					backEntry.resize(length);

					length = getResultFn(instanceHandle, i, const_cast<char*>(backEntry.c_str()), length);
					if (length <= 0) {
						/* Error */
						isOk = (DllAdapterStatus)(DLL_ADAPTER_STATUS_ERROR_INTERNAL + 3000 - length);
					}
					else {
						/* Strip null terminator */
						if (*backEntry.rbegin() == '\0') {
							backEntry.resize(length - 1);
						}
					}
				}
			}
		}
	}

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	functionMutex.unlock();
#endif

	return isOk;
}

void DllAdapter::GetDllVersion(int& majorVersion, int& minorVersion, int& buildNumber) {
	if (getDllVersionFn == NULL) {
		/* Not set up yet */
		majorVersion = -1;
		minorVersion = -1;
		buildNumber = -1;
	}
	else {
		getDllVersionFn(&majorVersion, &minorVersion, &buildNumber);
	}
}

void DllAdapter::GetErrorText(std::ostream& stream, const DllAdapterStatus error) {
	switch (error) {
	case DLL_ADAPTER_STATUS_SUCCESS:
		stream << "Success";
		break;
	case DLL_ADAPTER_STATUS_ERROR_DLL_MISSING:
		stream << "ERROR: Controller interface DLL is missing";
		break;
	case DLL_ADAPTER_STATUS_ERROR_DLL_INVALID:
		stream << "ERROR: Controller interface DLL is present but does not have expected functions";
		break;
	case DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP:
		stream << "ERROR: Interface was not set up before carrying out operations";
		break;
	case DLL_ADAPTER_STATUS_ERROR_PORT_NOT_AVAILABLE:
		stream << "ERROR: Port not available";
		break;
	case DLL_ADAPTER_STATUS_ERROR_STAGE_NOT_AVAILABLE:
		stream << "ERROR: Stage not available";
		break;
	case DLL_ADAPTER_STATUS_ERROR_UNKNOWN_COMMAND:
		stream << "ERROR: Unknown command";
		break;
	case DLL_ADAPTER_STATUS_ERROR_INCORRECT_PARAMS:
		stream << "ERROR: Incorrect parameters for command";
		break;
	case DLL_ADAPTER_STATUS_ERROR_COMMAND_NOT_SENT:
		stream << "ERROR: Command could not be sent/received";
		break;
	default:
		if (error > DLL_ADAPTER_STATUS_ERROR_INTERNAL) {
			stream << "ERROR: Internal error " << ((int)error - (int)DLL_ADAPTER_STATUS_ERROR_INTERNAL);
		}
		else if (error > DLL_ADAPTER_STATUS_ERROR_COMMS_ERROR) {
			stream << "ERROR: Comms error " << ((int)error - (int)DLL_ADAPTER_STATUS_ERROR_COMMS_ERROR);
		}
		else {
			stream << "ERROR: Unknown error type " << (int)error;
		}
		break;
	};
}

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
/* Windows */

DllAdapterStatus DllAdapter::InitDllInterface(const std::string& dllPath) {
	/* Open DLL */
	HMODULE handle;
	handle = LoadLibraryA(dllPath.c_str());
	dllHandle = (void*)handle;
	if (handle == NULL) {
		return DLL_ADAPTER_STATUS_ERROR_DLL_MISSING;
	}

	/* Get functions */
	initFn = (InitFn)GetProcAddress(handle, "Init");
	findDevicesFn = (FindDevicesFn)GetProcAddress(handle, "FindDevices");
	getDeviceFn = (GetDeviceFn)GetProcAddress(handle, "GetDevice");
	openSessionFn = (OpenSessionFn)GetProcAddress(handle, "OpenSession");
	openSessionDatalinkFn = (OpenSessionDatalinkFn)GetProcAddress(handle, "OpenSessionDatalink");
	closeSessionFn = (CloseSessionFn)GetProcAddress(handle, "CloseSession");
	uninitFn = (UninitFn)GetProcAddress(handle, "Uninit");
	getChannelsFn = (GetChannelsFn)GetProcAddress(handle, "GetChannels");
	findCommandsFn = (FindCommandsFn)GetProcAddress(handle, "FindCommands");
	getCommandFn = (GetCommandFn)GetProcAddress(handle, "GetCommand");
	getCommandDescriptionFn = (GetCommandDescriptionFn)GetProcAddress(handle, "GetCommandDescription");
	getCommandParametersFn = (GetCommandParametersFn)GetProcAddress(handle, "GetCommandParameters");
	getCommandParameterNameFn = (GetCommandParameterNameFn)GetProcAddress(handle, "GetCommandParameterName");
	getCommandParameterUnitsTypeFn = (GetCommandParameterUnitsTypeFn)GetProcAddress(handle, "GetCommandParameterUnitsType");
	getCommandParameterUnitsFn = (GetCommandParameterUnitsFn)GetProcAddress(handle, "GetCommandParameterUnits");
	getCommandResultsFn = (GetCommandResultsFn)GetProcAddress(handle, "GetCommandResults");
	getCommandResultNameFn = (GetCommandResultNameFn)GetProcAddress(handle, "GetCommandResultName");
	getCommandResultUnitsTypeFn = (GetCommandResultUnitsTypeFn)GetProcAddress(handle, "GetCommandResultUnitsType");
	getCommandResultUnitsFn = (GetCommandResultUnitsFn)GetProcAddress(handle, "GetCommandResultUnits");
	getEnumerationsFn = (GetEnumerationsFn)GetProcAddress(handle, "GetEnumerations");
	getEnumerationNameFn = (GetEnumerationNameFn)GetProcAddress(handle, "GetEnumerationName");
	getEnumerationValuesFn = (GetEnumerationValuesFn)GetProcAddress(handle, "GetEnumerationValues");
	getEnumerationValueFn = (GetEnumerationValueFn)GetProcAddress(handle, "GetEnumerationValue");
	doCommandFn = (DoCommandFn)GetProcAddress(handle, "DoCommand");
	getResultNameFn = (GetResultNameFn)GetProcAddress(handle, "GetResultName");
	getResultFn = (GetResultFn)GetProcAddress(handle, "GetResult");
	getDllVersionFn = (GetDllVersionFn)GetProcAddress(handle, "GetDllVersion");

	if ((initFn == NULL) ||
		(findDevicesFn == NULL) ||
		(getDeviceFn == NULL) ||
		(openSessionFn == NULL) ||
		(closeSessionFn == NULL) ||
		(uninitFn == NULL) ||
		(getChannelsFn == NULL) ||
		(findCommandsFn == NULL) ||
		(getCommandFn == NULL) ||
		(getCommandDescriptionFn == NULL) ||
		(getCommandParametersFn == NULL) ||
		(getCommandParameterNameFn == NULL) ||
		(getCommandParameterUnitsTypeFn == NULL) ||
		(getCommandParameterUnitsFn == NULL) ||
		(getCommandResultsFn == NULL) ||
		(getCommandResultNameFn == NULL) ||
		(getCommandResultUnitsTypeFn == NULL) ||
		(getCommandResultUnitsFn == NULL) ||
		(doCommandFn == NULL) ||
		(getResultNameFn == NULL) ||
		(getResultFn == NULL) ||
		(getDllVersionFn == NULL)) {
		return DLL_ADAPTER_STATUS_ERROR_DLL_INVALID;
	}
	else {
		/* Note that other functions could legitimately be NULL for older DLLs */
		return DLL_ADAPTER_STATUS_SUCCESS;
	}
}

void DllAdapter::FreeDllInterface() {
	if (dllHandle != NULL) {
		HMODULE handle = (HMODULE)dllHandle;
		FreeLibrary(handle);
	}

	initFn = NULL;
	findDevicesFn = NULL;
	getDeviceFn = NULL;
	openSessionFn = NULL;
	openSessionDatalinkFn = NULL;
	closeSessionFn = NULL;
	uninitFn = NULL;
	getChannelsFn = NULL;
	findCommandsFn = NULL;
	getCommandFn = NULL;
	getCommandDescriptionFn = NULL;
	getCommandParametersFn = NULL;
	getCommandParameterNameFn = NULL;
	getCommandParameterUnitsTypeFn = NULL;
	getCommandParameterUnitsFn = NULL;
	getCommandResultsFn = NULL;
	getCommandResultNameFn = NULL;
	getCommandResultUnitsTypeFn = NULL;
	getCommandResultUnitsFn = NULL;
	getEnumerationsFn = NULL;
	getEnumerationNameFn = NULL;
	getEnumerationValuesFn = NULL;
	getEnumerationValueFn = NULL;
	doCommandFn = NULL;
	getResultNameFn = NULL;
	getResultFn = NULL;
	getDllVersionFn = NULL;

	dllHandle = NULL;
}

#else
/* Linux/Posix */

void* getDllFunction(void* const handle, const char* const name) {
	void* funcPtr;
	char* error;

	funcPtr = dlsym(handle, name);
	error = dlerror();
	if (error != NULL) {
		funcPtr = NULL;
	}

	return funcPtr;
}

DllAdapterStatus DllAdapter::InitDllInterface(const std::string& dllPath) {
	/* Open DLL */
	dllHandle = dlopen(dllPath.c_str(), RTLD_LOCAL | RTLD_NOW);
	if (dllHandle == NULL) {
		return DLL_ADAPTER_STATUS_ERROR_DLL_MISSING;
	}

	/* Get functions */
	initFn = (InitFn)getDllFunction(dllHandle, "Init");
	findDevicesFn = (FindDevicesFn)getDllFunction(dllHandle, "FindDevices");
	getDeviceFn = (GetDeviceFn)getDllFunction(dllHandle, "GetDevice");
	openSessionFn = (OpenSessionFn)getDllFunction(dllHandle, "OpenSession");
	openSessionDatalinkFn = (OpenSessionDatalinkFn)getDllFunction(dllHandle, "OpenSessionDatalink");
	closeSessionFn = (CloseSessionFn)getDllFunction(dllHandle, "CloseSession");
	uninitFn = (UninitFn)getDllFunction(dllHandle, "Uninit");
	getChannelsFn = (GetChannelsFn)getDllFunction(dllHandle, "GetChannels");
	findCommandsFn = (FindCommandsFn)getDllFunction(dllHandle, "FindCommands");
	getCommandFn = (GetCommandFn)getDllFunction(dllHandle, "GetCommand");
	getCommandDescriptionFn = (GetCommandDescriptionFn)getDllFunction(dllHandle, "GetCommandDescription");
	getCommandParametersFn = (GetCommandParametersFn)getDllFunction(dllHandle, "GetCommandParameters");
	getCommandParameterNameFn = (GetCommandParameterNameFn)getDllFunction(dllHandle, "GetCommandParameterName");
	getCommandParameterUnitsTypeFn = (GetCommandParameterUnitsTypeFn)getDllFunction(dllHandle, "GetCommandParameterUnitsType");
	getCommandParameterUnitsFn = (GetCommandParameterUnitsFn)getDllFunction(dllHandle, "GetCommandParameterUnits");
	getCommandResultsFn = (GetCommandResultsFn)getDllFunction(dllHandle, "GetCommandResults");
	getCommandResultNameFn = (GetCommandResultNameFn)getDllFunction(dllHandle, "GetCommandResultName");
	getCommandResultUnitsTypeFn = (GetCommandResultUnitsTypeFn)getDllFunction(dllHandle, "GetCommandResultUnitsType");
	getCommandResultUnitsFn = (GetCommandResultUnitsFn)getDllFunction(dllHandle, "GetCommandResultUnits");
	getEnumerationsFn = (GetEnumerationsFn)getDllFunction(dllHandle, "GetEnumerations");
	getEnumerationNameFn = (GetEnumerationNameFn)getDllFunction(dllHandle, "GetEnumerationName");
	getEnumerationValuesFn = (GetEnumerationValuesFn)getDllFunction(dllHandle, "GetEnumerationValues");
	getEnumerationValueFn = (GetEnumerationValueFn)getDllFunction(dllHandle, "GetEnumerationValue");
	doCommandFn = (DoCommandFn)getDllFunction(dllHandle, "DoCommand");
	getResultNameFn = (GetResultNameFn)getDllFunction(dllHandle, "GetResultName");
	getResultFn = (GetResultFn)getDllFunction(dllHandle, "GetResult");
	getDllVersionFn = (GetDllVersionFn)getDllFunction(dllHandle, "GetDllVersion");

	if ((initFn == NULL) ||
		(findDevicesFn == NULL) ||
		(getDeviceFn == NULL) ||
		(openSessionFn == NULL) ||
		(closeSessionFn == NULL) ||
		(uninitFn == NULL) ||
		(getChannelsFn == NULL) ||
		(findCommandsFn == NULL) ||
		(getCommandFn == NULL) ||
		(getCommandDescriptionFn == NULL) ||
		(getCommandParametersFn == NULL) ||
		(getCommandParameterNameFn == NULL) ||
		(getCommandParameterUnitsTypeFn == NULL) ||
		(getCommandParameterUnitsFn == NULL) ||
		(getCommandResultsFn == NULL) ||
		(getCommandResultNameFn == NULL) ||
		(getCommandResultUnitsTypeFn == NULL) ||
		(getCommandResultUnitsFn == NULL) ||
		(doCommandFn == NULL) ||
		(getResultNameFn == NULL) ||
		(getResultFn == NULL) ||
		(getDllVersionFn == NULL)) {
		return DLL_ADAPTER_STATUS_ERROR_DLL_INVALID;
	}
	else {
		/* Note that other functions could legitimately be NULL for older DLLs */
		return DLL_ADAPTER_STATUS_SUCCESS;
	}
}

void DllAdapter::FreeDllInterface() {
	if (dllHandle != NULL) {
		dlclose(dllHandle);
	}

	initFn = NULL;
	findDevicesFn = NULL;
	getDeviceFn = NULL;
	openSessionFn = NULL;
	openSessionDatalinkFn = NULL;
	closeSessionFn = NULL;
	uninitFn = NULL;
	getChannelsFn = NULL;
	findCommandsFn = NULL;
	getCommandFn = NULL;
	getCommandDescriptionFn = NULL;
	getCommandParametersFn = NULL;
	getCommandParameterNameFn = NULL;
	getCommandParameterUnitsTypeFn = NULL;
	getCommandParameterUnitsFn = NULL;
	getCommandResultsFn = NULL;
	getCommandResultNameFn = NULL;
	getCommandResultUnitsTypeFn = NULL;
	getCommandResultUnitsFn = NULL;
	getEnumerationsFn = NULL;
	getEnumerationNameFn = NULL;
	getEnumerationValuesFn = NULL;
	getEnumerationValueFn = NULL;
	doCommandFn = NULL;
	getResultNameFn = NULL;
	getResultFn = NULL;
	getDllVersionFn = NULL;

	dllHandle = NULL;
}

#endif