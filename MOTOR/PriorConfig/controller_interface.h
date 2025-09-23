/**
@file controller_interface.h
Controller interface flat C-style API.

For every controller connection, it is necessary to get an interface handle with
Init.  The interface handle is passed to functions so that operations happen on
the correct connection.  Once the connection is no longer in use, the handle 
must be freed with Uninit.

@authors Graham Bartlett
@copyright Prior Scientific Instruments Ltd., 2015
*/

#ifndef CONTROLLER_INTERFACE_H
#define CONTROLLER_INTERFACE_H

#include "et_common_api.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifndef INTERFACE_API
#define INTERFACE_API extern
#endif

/** Handle to controller interface instance.
 */
typedef void* ControllerInterfaceHandle;

/** Initialise controller interface.
 *
 * Each instantiation of the DLL communicates with a single connection.  Each
 * must be initialised by calling this function before it is used.
 * @returns Handle to controller interface instance
 */
INTERFACE_API ControllerInterfaceHandle Init();

/** Handle to Init function.
* @returns Handle to controller interface instance
*/
typedef ControllerInterfaceHandle(*InitFn)();

/** Get number of controllers currently connected.
 * @param handle Handle to controller interface instance allocated with Init
 * @returns Number of controllers currently connected
 *
 * @note This function is currently unimplemented and always returns zero.
 */
INTERFACE_API int FindDevices(ControllerInterfaceHandle handle);

/** Handle to FindDevices function.
* @param handle Handle to controller interface instance allocated with Init
* @returns Number of controllers currently connected
 */
typedef int(*FindDevicesFn)(ControllerInterfaceHandle handle);

/** Get URI of connected controller from list.
 * @param handle Handle to controller interface instance allocated with Init
 * @param index Controller index (zero-based)
 * @param buffer String to contain controller URI (may be NULL if the purpose is only to find the controller URI length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the controller URI length)
 * @returns Length of controller URI, or negative in case of error
 *
 * @note This function is currently unimplemented and always returns zero.
 */
INTERFACE_API int GetDevice(ControllerInterfaceHandle handle, const int index, char* buffer, const int bufferLen);

/** Handle to GetDevice function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param index Controller index (zero-based)
 * @param buffer String to contain controller URI (may be NULL if the purpose is only to find the controller URI length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the controller URI length)
 * @returns Length of controller URI, or negative in case of error
 */
typedef int(*GetDeviceFn)(ControllerInterfaceHandle handle, const int index, char* buffer, const int bufferLen);

/** Attempt to open a session to a controller on the specified device.
 * @param handle Handle to controller interface instance allocated with Init
 * @param device Controller URI
 * @returns 1 if successful, 0 if no controller found
 */
INTERFACE_API int OpenSession(ControllerInterfaceHandle handle, const char* device);

/** Handle to OpenSession function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param device Controller URI
 * @returns 1 if successful, 0 if no controller found
 */
typedef int(*OpenSessionFn)(ControllerInterfaceHandle handle, const char* device);

/** Function pointer type for datalink read.
 *
 * This function must block until either minimumReadLength bytes (or greater, up
 * to readBufferLength) have been read from the datalink layer, or until
 * readTimeoutMs time has expired (if readTimeoutMs is non-zero), or until an 
 * error occurs.
 *
 * If minimumReadLength is zero, the function must read any bytes available (up to
 * readBufferLength) and return immediately.  It must not block in this case.
 *
 * @param readBuffer Buffer in which bytes read from datalink layer will be stored
 * @param readBufferLength Length of readBuffer in bytes
 * @param minimumReadLength Minimum number of bytes to read from datalink layer before unblocking.  If zero, return immediately with any data in buffers (i.e. do not block).
 * @param readTimeoutMs Timeout in ms when reading datalink layer.  If zero, return immediately (i.e. do not block).
 * @param contextPtr Optional void pointer for datalink context
 * @param contextVal Optional integer for datalink context
 * @returns Number of bytes read (zero if no bytes read in readTimeoutMs time), or negative if there is an error
 */
typedef int(*ControllerInterfaceDatalinkReadFn)(char* readBuffer,
											const unsigned int readBufferLength,
											const unsigned int minimumReadLength,
											const unsigned int readTimeoutMs,
											void* contextPtr,
											int contextVal);

/** Function pointer type for datalink write.
 *
 * This function may use a synchronous write and block until the write is complete,
 * or it may use an asynchronous write and return as soon as the write is started.
 * If using an asynchronous write, there
 *
 * @param handle Handle to datalink
 * @param writeBuffer Buffer in which bytes to write to datalink layer are stored
 * @param writeBufferLength Number of bytes to write to datalink layer
 * @param contextPtr Optional void pointer for datalink context
 * @param contextVal Optional integer for datalink context
 * @returns Zero if successful, or negative if there is an error
 */
typedef int(*ControllerInterfaceDatalinkWriteFn)(const char* writeBuffer,
											const unsigned int writeBufferLength,
											void* contextPtr,
											int contextVal);

/** Attempt to open a session to a controller with user-specified datalink read and write functions.
 *
 * This allows the DLL to be interfaced to any other I/O, or with other applications/frameworks
 * which have particular APIs for controlling I/O.  Context data can be provided to pass to
 * the relevant read and write functions.
 *
 * @param handle Handle to controller interface instance allocated with Init
 * @param datalinkRead Function for datalink read
 * @param datalinkWrite Function for datalink write
 * @param contextPtr Optional void pointer for datalink context
 * @param contextVal Optional integer for datalink context
 * @returns 1 if successful, 0 if no controller found
 */
INTERFACE_API int OpenSessionDatalink(ControllerInterfaceHandle handle, 
								ControllerInterfaceDatalinkReadFn datalinkRead,
								ControllerInterfaceDatalinkWriteFn datalinkWrite,
								void* contextPtr,
								int contextVal);

/** Handle to OpenSessionDatalink function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param datalinkRead Function for datalink read
 * @param datalinkWrite Function for datalink write
 * @param contextPtr Optional void pointer for datalink context
 * @param contextVal Optional integer for datalink context
 * @returns 1 if successful, 0 if no controller found
 */
typedef int(*OpenSessionDatalinkFn)(ControllerInterfaceHandle handle, 
								ControllerInterfaceDatalinkReadFn datalinkRead,
								ControllerInterfaceDatalinkWriteFn datalinkWrite,
								void* contextPtr,
								int contextVal);

/** Close current session.
 * @param handle Handle to controller interface instance allocated with Init
 */
INTERFACE_API void CloseSession(ControllerInterfaceHandle handle);

/** Handle to CloseSession function.
 * @param handle Handle to controller interface instance allocated with Init
 */
typedef void(*CloseSessionFn)(ControllerInterfaceHandle handle);

/** Tear down controller interface.
 * @param handle Handle to controller interface instance allocated with Init
 */
INTERFACE_API void Uninit(ControllerInterfaceHandle handle);

/** Handle to Uninit function.
 * @param handle Handle to controller interface instance allocated with Init
 */
typedef void(*UninitFn)(ControllerInterfaceHandle handle);

/** Get number of channels on controller
 * @param handle Handle to controller interface instance allocated with Init
 * @returns Number of channels, or negative in case of error
 */
INTERFACE_API int GetChannels(ControllerInterfaceHandle handle);

/** Handle to GetChannels function.
 * @param handle Handle to controller interface instance allocated with Init
 * @returns Number of channels, or negative in case of error
 */
typedef int(*GetChannelsFn)(ControllerInterfaceHandle handle);

/** Get number of commands supported on the connected controller for the
 * current security level.
 * @param handle Handle to controller interface instance allocated with Init
 * @param filter Filter for commands starting with this text
 * @returns Number of commands; or negative if there was an error
 *
 * @note The DLL does not currently support filtering, so the filter parameter
 * is not used.  This function (and GetCommand and GetDescription) report all
 * supported commands.
 */
INTERFACE_API int FindCommands(ControllerInterfaceHandle handle, const char* filter);

/** Handle to FindCommands function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param filter Filter for commands starting with this text
 * @returns Number of commands; or negative if there was an error
 */
typedef int(*FindCommandsFn)(ControllerInterfaceHandle handle, const char* filter);

/** Get command name from list.
 * @param handle Handle to controller interface instance allocated with Init
 * @param index Command index (zero-based)
 * @param buffer String to contain command name (may be NULL if the purpose is only to find the command name length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the command name length)
 * @returns Length of command name, or negative in case of error
 */
INTERFACE_API int GetCommand(ControllerInterfaceHandle handle, const int index, char* buffer, const int bufferLen);

/** Handle to GetCommand function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param index Command index (zero-based)
 * @param buffer String to contain command name (may be NULL if the purpose is only to find the command name length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the command name length)
 * @returns Length of command name; or negative if there was an error
 */
typedef int(*GetCommandFn)(ControllerInterfaceHandle handle, const int index, char* buffer, const int bufferLen);

/** Get command description.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param buffer String to contain command description (may be NULL if the purpose is only to find the command description length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the command description length)
 * @returns Length of command description, or negative in case of error
 */
INTERFACE_API int GetCommandDescription(ControllerInterfaceHandle handle, const char* commandName, char* buffer, const int bufferLen);

/** Handle to GetDescription function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param buffer String to contain command description (may be NULL if the purpose is only to find the command description length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the command description length)
 * @returns Length of command description, or negative in case of error
 */
typedef int(*GetCommandDescriptionFn)(ControllerInterfaceHandle handle, const char* commandName, char* buffer, const int bufferLen);

/** Get number of parameters for command.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @returns Number of command parameters (which may be zero), or negative in case of error
 */
INTERFACE_API int GetCommandParameters(ControllerInterfaceHandle handle, const char* commandName);

/** Handle to GetCommandParameters function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @returns Number of command parameters (which may be zero), or negative in case of error
 */
typedef int(*GetCommandParametersFn)(ControllerInterfaceHandle handle, const char* commandName);

/** Get command parameter name.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param paramIndex Parameter index (zero-based)
 * @param buffer String to contain parameter name (may be NULL if the purpose is only to find the parameter name length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the parameter name length)
 * @returns Length of parameter name, or negative in case of error
 */
INTERFACE_API int GetCommandParameterName(ControllerInterfaceHandle handle, const char* commandName, const int paramIndex, char* buffer, const int bufferLen);

/** Handle to GetCommandParameterName function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param paramIndex Parameter index (zero-based)
 * @param buffer String to contain parameter name (may be NULL if the purpose is only to find the parameter name length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the parameter name length)
 * @returns Length of parameter name, or negative in case of error
 */
typedef int(*GetCommandParameterNameFn)(ControllerInterfaceHandle handle, const char* commandName, const int paramIndex, char* buffer, const int bufferLen);

/** Get command parameter units type (e.g. "distance").
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param paramIndex Parameter index (zero-based)
 * @param channel Channel number (1-based)
 * @param buffer String to contain parameter type (may be NULL if the purpose is only to find the parameter type length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the parameter type length)
 * @returns Length of parameter type, or negative in case of error
 */
INTERFACE_API int GetCommandParameterUnitsType(ControllerInterfaceHandle handle, const char* commandName, const int paramIndex, const int channel, char* buffer, const int bufferLen);

/** Handle to GetCommandParameterUnitsType function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param paramIndex Parameter index (zero-based)
 * @param channel Channel number (1-based)
 * @param buffer String to contain parameter type (may be NULL if the purpose is only to find the parameter type length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the parameter type length)
 * @returns Length of parameter type, or negative in case of error
 */
typedef int(*GetCommandParameterUnitsTypeFn)(ControllerInterfaceHandle handle, const char* commandName, const int paramIndex, const int channel, char* buffer, const int bufferLen);

/** Get command parameter units (e.g. "pm").
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param paramIndex Parameter index (zero-based)
 * @param channel Channel number (1-based)
 * @param buffer String to contain parameter units (may be NULL if the purpose is only to find the parameter units length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the parameter units length)
 * @returns Length of parameter units, or negative in case of error
 */
INTERFACE_API int GetCommandParameterUnits(ControllerInterfaceHandle handle, const char* commandName, const int paramIndex, const int channel, char* buffer, const int bufferLen);

/** Handle to GetCommandParameterUnits function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param paramIndex Parameter index (zero-based)
 * @param channel Channel number (1-based)
 * @param buffer String to contain parameter units (may be NULL if the purpose is only to find the parameter units length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the parameter units length)
 * @returns Length of parameter units, or negative in case of error
 */
typedef int(*GetCommandParameterUnitsFn)(ControllerInterfaceHandle handle, const char* commandName, const int paramIndex, const int channel, char* buffer, const int bufferLen);

/** Get number of results for command if command succeeds.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @returns Number of command results (which will always be greater than zero), or negative in case of error
 */
INTERFACE_API int GetCommandResults(ControllerInterfaceHandle handle, const char* commandName);

/** Handle to GetCommandResults function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @returns Number of command results (which will always be greater than zero), or negative in case of error
 */
typedef int(*GetCommandResultsFn)(ControllerInterfaceHandle handle, const char* commandName);

/** Get command result name.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param resultIndex Result index (zero-based)
 * @param buffer String to contain result name (may be NULL if the purpose is only to find the result name length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the result name length)
 * @returns Length of result name, or negative in case of error
 */
INTERFACE_API int GetCommandResultName(ControllerInterfaceHandle handle, const char* commandName, const int resultIndex, char* buffer, const int bufferLen);

/** Handle to GetCommandResultName function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param resultIndex Result index (zero-based)
 * @param buffer String to contain result name (may be NULL if the purpose is only to find the result name length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the result name length)
 * @returns Length of result name, or negative in case of error
 */
typedef int(*GetCommandResultNameFn)(ControllerInterfaceHandle handle, const char* commandName, const int resultIndex, char* buffer, const int bufferLen);

/** Get command result units type (e.g. "distance").
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param resultIndex Result index (zero-based)
 * @param channel Channel number (1-based)
 * @param buffer String to contain result type (may be NULL if the purpose is only to find the result type length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the result type length)
 * @returns Length of result type, or negative in case of error
 */
INTERFACE_API int GetCommandResultUnitsType(ControllerInterfaceHandle handle, const char* commandName, const int resultIndex, const int channel, char* buffer, const int bufferLen);

/** Handle to GetCommandResultType function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param resultIndex Result index (zero-based)
 * @param channel Channel number (1-based)
 * @param buffer String to contain result type (may be NULL if the purpose is only to find the result type length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the result type length)
 * @returns Length of result type, or negative in case of error
 */
typedef int(*GetCommandResultUnitsTypeFn)(ControllerInterfaceHandle handle, const char* commandName, const int resultIndex, const int channel, char* buffer, const int bufferLen);

/** Get command result units (e.g. "pm").
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param resultIndex Result index
 * @param channel Channel number (1-based)
 * @param buffer String to contain result units (may be NULL if the purpose is only to find the result units length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the result units length)
 * @returns Length of result units, or negative in case of error
 */
INTERFACE_API int GetCommandResultUnits(ControllerInterfaceHandle handle, const char* commandName, const int resultIndex, const int channel, char* buffer, const int bufferLen);

/** Handle to GetCommandResultUnits function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param commandName Command name
 * @param resultIndex Result index (zero-based)
 * @param channel Channel number (1-based)
 * @param buffer String to contain result units (may be NULL if the purpose is only to find the result units length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the result units length)
 * @returns Length of result units, or negative in case of error
 */
typedef int(*GetCommandResultUnitsFn)(ControllerInterfaceHandle handle, const char* commandName, const int resultIndex, const int channel, char* buffer, const int bufferLen);

/** Get number of enumerations available for controller.
 * @param handle Handle to controller interface instance allocated with Init
 * @returns Number of enumerations available, or negative in case of error
 */
INTERFACE_API int GetEnumerations(ControllerInterfaceHandle handle);

/** Handle to GetEnumerations function.
 * @param handle Handle to controller interface instance allocated with Init
 * @returns Number of enumerations available, or negative in case of error
 */
typedef int (*GetEnumerationsFn)(ControllerInterfaceHandle handle);

/** Get enumeration name.
 * @param handle Handle to controller interface instance allocated with Init
 * @param index Enumeration index (zero-based)
 * @param buffer String to contain enumeration name (may be NULL if the purpose is only to find the enumeration name length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the enumeration name length)
 * @returns Length of enumeration name, or negative in case of error
 */
INTERFACE_API int GetEnumerationName(ControllerInterfaceHandle handle, const int index,  char * buffer, const int bufferLen);

/** Handle to GetEnumerationName function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param index Enumeration index (zero-based)
 * @param buffer String to contain enumeration name (may be NULL if the purpose is only to find the enumeration name length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the enumeration name length)
 * @returns Length of enumeration name, or negative in case of error
 */
typedef int (*GetEnumerationNameFn)(ControllerInterfaceHandle handle, const int index,  char * buffer, const int bufferLen);

/** Get number of values for enumeration.
 * @param handle Handle to controller interface instance allocated with Init
 * @param enumerationName Enumeration name
 * @returns Number of enumeration values, or negative in case of error
 */
INTERFACE_API int GetEnumerationValues(ControllerInterfaceHandle handle, const char * enumerationName);

/** Handle to GetEnumerationValues function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param enumerationName Enumeration name
 * @returns Number of enumeration values, or negative in case of error
 */
typedef int (*GetEnumerationValuesFn)(ControllerInterfaceHandle handle, const char * enumerationName);

/** Get enumeration value.
 * @param handle Handle to controller interface instance allocated with Init
 * @param enumerationName Enumeration name
 * @param index Enumeration value index (zero-based)
 * @param buffer String to contain enumeration value (may be NULL if the purpose is only to find the enumeration value length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the enumeration value length)
 * @returns Length of enumeration value, or negative in case of error
 */
INTERFACE_API int GetEnumerationValue(ControllerInterfaceHandle handle, const char * enumerationName, const int index, char * buffer, const int bufferLen);

/** Handle to GetEnumerationValue function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param enumerationName Enumeration name
 * @param index Enumeration value index (zero-based)
 * @param buffer String to contain enumeration value (may be NULL if the purpose is only to find the enumeration value length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the enumeration value length)
 * @returns Length of enumeration value, or negative in case of error
 */
typedef int (*GetEnumerationValueFn)(ControllerInterfaceHandle handle, const char * enumerationName, const int index, char * buffer, const int bufferLen);

/** Send command(s) to controller.
 *
 * This function blocks until a response (if any) has been received
 * and processed.
 *
 * @param handle Handle to controller interface instance allocated with Init
 * @param command Command string
 * @returns Positive if successful; zero or negative if failed
 */
INTERFACE_API int DoCommand(ControllerInterfaceHandle handle, const char* command);

/** Handle to DoCommand function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param command Command string
 * @returns Positive if successful; zero or negative if failed
 */
typedef int(*DoCommandFn)(ControllerInterfaceHandle handle, const char* command);

/** Get name of result for last commands sent with DoCommand.
 * @param handle Handle to controller interface instance allocated with Init
 * @param index Result index (zero-based)
 * @param buffer String to contain result name (may be NULL if the purpose is only to find the result name length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the result name length)
 * @returns Length of result name, or negative in case of error
 */
INTERFACE_API int GetResultName(ControllerInterfaceHandle handle, const int index, char* buffer, const int bufferLen);

/** Handle to GetResultName function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param index Result index (zero-based)
 * @param buffer String to contain result name (may be NULL if the purpose is only to find the result name length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the result name length)
 * @returns Length of result name, or negative in case of error
 */
typedef int(*GetResultNameFn)(ControllerInterfaceHandle handle, const int index, char* buffer, const int bufferLen);

/** Get names of all results for last commands sent with DoCommand, separated with newlines.
* @param handle Handle to controller interface instance allocated with Init
* @param buffer String to contain result names (may be NULL if the purpose is only to find the result name length)
* @param bufferLen Length of buffer (may be zero if the purpose is only to find the result name length)
* @returns Length of result name, or negative in case of error
*/
INTERFACE_API int GetAllResultNames(ControllerInterfaceHandle handle, char* buffer, const int bufferLen);

/** Handle to GetAllResultNames function.
* @param handle Handle to controller interface instance allocated with Init
* @param buffer String to contain result names (may be NULL if the purpose is only to find the result name length)
* @param bufferLen Length of buffer (may be zero if the purpose is only to find the result name length)
* @returns Length of result name, or negative in case of error
*/
typedef int(*GetAllResultNamesFn)(ControllerInterfaceHandle handle, char* buffer, const int bufferLen);

/** Get result value for last command sent with DoCommand.
 * 
 * This may be a number (floating-point, integer or hexadecimal), or may be
 * a string if this is more appropriate.
 *
 * @param handle Handle to controller interface instance allocated with Init
 * @param index Result index (zero-based)
 * @param buffer String to contain result value (may be NULL if the purpose is only to find the result value length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the result value length)
 * @returns Length of result value, or negative in case of error
 */
INTERFACE_API int GetResult(ControllerInterfaceHandle handle, const int index, char* buffer, const int bufferLen);

/** Handle to GetResult function.
 * @param handle Handle to controller interface instance allocated with Init
 * @param index Result index (zero-based)
 * @param buffer String to contain result value (may be NULL if the purpose is only to find the result value length)
 * @param bufferLen Length of buffer (may be zero if the purpose is only to find the result value length)
 * @returns Length of result value, or negative in case of error
 */
typedef int(*GetResultFn)(ControllerInterfaceHandle handle, const int index, char* buffer, const int bufferLen);

/** Get all result values for last commands sent with DoCommand, separated with newlines.
* @param handle Handle to controller interface instance allocated with Init
* @param buffer String to contain result values (may be NULL if the purpose is only to find the result value length)
* @param bufferLen Length of buffer (may be zero if the purpose is only to find the result value length)
* @returns Length of result value, or negative in case of error
*/
INTERFACE_API int GetAllResults(ControllerInterfaceHandle handle, char* buffer, const int bufferLen);

/** Handle to GetAllResults function.
* @param handle Handle to controller interface instance allocated with Init
* @param buffer String to contain result values (may be NULL if the purpose is only to find the result value length)
* @param bufferLen Length of buffer (may be zero if the purpose is only to find the result value length)
* @returns Length of result value, or negative in case of error
*/
typedef int(*GetAllResultsFn)(ControllerInterfaceHandle handle, char* buffer, const int bufferLen);

/** Get DLL release version.
 * @param majorVersion Major version number
 * @param minorVersion Minor version number
 * @param buildNumber Build number
 */
INTERFACE_API void GetDllVersion(int* majorVersion, int* minorVersion, int* buildNumber);

/** Handle to GetDllVersion function.
 * @param majorVersion Major version number
 * @param minorVersion Minor version number
 * @param buildNumber Build number
 */
typedef void(*GetDllVersionFn)(int* majorVersion, int* minorVersion, int* buildNumber);


#ifdef __cplusplus
}
#endif

#endif
