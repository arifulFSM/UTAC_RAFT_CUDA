/**
@file dll_adapter.hpp
Defines adapter around command interface DLL.

@authors Graham Bartlett
@copyright Prior Scientific Instruments Ltd., 2016
*/

#ifndef DLL_ADAPTER_H
#define DLL_ADAPTER_H

#include "controller_interface.h"

#include <string>
#include <list>

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
#include "tinythread.h"
#endif


/** Return status.
 */
enum DllAdapterStatus
{
    DLL_ADAPTER_STATUS_SUCCESS = 0,
    DLL_ADAPTER_STATUS_ERROR_DLL_MISSING,
    DLL_ADAPTER_STATUS_ERROR_DLL_INVALID,
    DLL_ADAPTER_STATUS_ERROR_NOT_SET_UP,
    DLL_ADAPTER_STATUS_ERROR_PORT_NOT_AVAILABLE,
    DLL_ADAPTER_STATUS_ERROR_STAGE_NOT_AVAILABLE,
    DLL_ADAPTER_STATUS_ERROR_UNKNOWN_COMMAND,
    DLL_ADAPTER_STATUS_ERROR_INCORRECT_PARAMS,
    DLL_ADAPTER_STATUS_ERROR_COMMAND_NOT_SENT,

    DLL_ADAPTER_STATUS_ERROR_COMMS_ERROR = 5000,

    DLL_ADAPTER_STATUS_ERROR_INTERNAL = 10000,
};


/** Adapter around command interface DLL.
 */
class DllAdapter
{
public:
    /** Constructor.
     *
     * Note: The adapter is **NOT** yet ready for use.  It must be initialised
     * by calling Init, which gives us a way to safely check for the interface
     * DLL existing and fail if necessary.
     */
    DllAdapter();

    /** Destructor.
     *`
     * Note that this is not virtual because we do not expect anyone to inherit
     * from this class.
     */
    ~DllAdapter();

    /** Initialise adapter, opening interface DLL for use.
     * @param dllPath Path and filename for DLL
     * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
     */
    DllAdapterStatus Init(const std::string& dllPath);

    /** Get controllers currently connected.
    * @param devices List of URIs to controllers currently connected
    * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
    */
    DllAdapterStatus GetDevices(std::list<std::string>& devices);

    /** Attempt to open a session to a controller on the specified device.
    * @param device Controller URI
    * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
    */
    DllAdapterStatus OpenSession(const std::string& device);

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
	 * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
	 */
	DllAdapterStatus OpenSessionDatalink(ControllerInterfaceDatalinkReadFn datalinkRead,
								ControllerInterfaceDatalinkWriteFn datalinkWrite,
								void* contextPtr,
								int contextVal);

    /** Close current session.
    */
    void CloseSession();

    /** Report whether session is open.
     * @returns True if session open
     */
    bool IsSessionOpen();

    /** Get number of channels on controller
    * @returns Number of channels, or negative in case of error
    */
    int GetChannels();

    /** Get commands supported on the connected controller for the
    * current security level.
    * @param commands List of commands
    * @param filter Filter for commands starting with this text
    * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
    */
    DllAdapterStatus FindCommands(std::list<std::string>& commands, const std::string& filter);

    /** Get command description.
    * @param commandName Command name
    * @param commandDescription Command description
    * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
    */
    DllAdapterStatus GetCommandDescription(const std::string& commandName, std::string& description);

    /** Get command parameters and units for each parameter.
    * @param commandName Command name
    * @param channel Channel index
    * @param commandParameterNames Command parameter names
    * @param commandParameterUnitTypes Command parameter unit types
    * @param commandParameterUnits Command parameter units
    * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
    */
    DllAdapterStatus GetCommandParameters(const std::string& commandName, const int channel, std::list<std::string>& commandParameterNames, std::list<std::string>& commandParameterUnitTypes, std::list<std::string>& commandParameterUnits);

    /** Get command results and units for each parameter.
    * @param commandName Command name
    * @param channel Channel index
    * @param commandResultNames Command result names
    * @param commandResultUnitTypes Command result unit types
    * @param commandResultUnits Command result units
    * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
    */
    DllAdapterStatus GetCommandResults(const std::string& commandName, const int channel, std::list<std::string>& commandResultNames, std::list<std::string>& commandResultUnitTypes, std::list<std::string>& commandResultUnits);

	/** Get enumerations available on the connected controller.
    * @param enumerations List of enumerations
    * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
    */
	DllAdapterStatus GetEnumerationNames(std::list<std::string>& enumerations);

	/** Get values available for an enumeration.
    * @param enumerationName Enumeration name
    * @param enumerationValues Enumeration values available
    * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
    */
	DllAdapterStatus GetEnumerationValues(const std::string& enumerationName, std::list<std::string>& enumerationValues);

    /** Send command(s) to controller.
    * @param command Command to send
    * @param resultNames Result names
    * @param resultValues Result values
    * @param isUnknown If command fails, notify user that this is due to command unknown and not other causes
    * @param isIncorrectParams If commands fails, notify user that this is due to incorrect parameters and not other causes
    * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
    */
    DllAdapterStatus DoCommand(const std::string& command, std::list<std::string>& resultNames, std::list<std::string>& resultValues);

    /** Get DLL release version.
    * @param majorVersion Major version number
    * @param minorVersion Minor version number
    * @param buildNumber Build number
    */
    void GetDllVersion(int& majorVersion, int& minorVersion, int& buildNumber);

    /** Stream text for error status to stream.
    * @param stream Stream for text
    * @param error Error status
    */
    static void GetErrorText(std::ostream& stream, const DllAdapterStatus error);

private:
    /** Explicitly prohibit copy constructor. */
    DllAdapter(const DllAdapter& copy);

protected:
    /** Initialise DLL interface for platform.
    * @param dllPath Path and filename for DLL
    * @returns DLL_ADAPTER_STATUS_SUCCESS if OK
    */
    DllAdapterStatus InitDllInterface(const std::string& dllPath);

    /** Free DLL interface for platform.
    */
    void FreeDllInterface();

#ifdef QUEENSGATE_DLL_ADAPTER_IS_THREAD_SAFE
	/** Optional mutex to make the adapter functions thread-safe */
	tthread::recursive_mutex functionMutex;
#endif

    /** Depending on the implementation, this could be the handle to the DLL or
     * could be a pointer to the handle to the DLL.
     */
    void* dllHandle;

    /** Handle to controller interface instance */
    ControllerInterfaceHandle instanceHandle;

    /** Store whether session has been opened */
    bool sessionIsOpen;

    /** Handle to Init function. */
    InitFn initFn;

    /** Handle to FindDevices function. */
    FindDevicesFn findDevicesFn;

    /** Handle to GetDevice function. */
    GetDeviceFn getDeviceFn;

    /** Handle to OpenSession function. */
    OpenSessionFn openSessionFn;

    /** Handle to OpenSessionDatalink function. */
    OpenSessionDatalinkFn openSessionDatalinkFn;

    /** Handle to CloseSession function. */
    CloseSessionFn closeSessionFn;

    /** Handle to Uninit function. */
    UninitFn uninitFn;

    /** Handle to GetChannels function. */
    GetChannelsFn getChannelsFn;

    /** Handle to FindCommands function. */
    FindCommandsFn findCommandsFn;

    /** Handle to GetCommand function. */
    GetCommandFn getCommandFn;

    /** Handle to GetDescription function. */
    GetCommandDescriptionFn getCommandDescriptionFn;

    /** Handle to GetCommandParameters function. */
    GetCommandParametersFn getCommandParametersFn;

    /** Handle to GetCommandParameterName function. */
    GetCommandParameterNameFn getCommandParameterNameFn;

    /** Handle to GetCommandParameterUnitsType function. */
    GetCommandParameterUnitsTypeFn getCommandParameterUnitsTypeFn;

    /** Handle to GetCommandParameterUnits function. */
    GetCommandParameterUnitsFn getCommandParameterUnitsFn;

    /** Handle to GetCommandResults function. */
    GetCommandResultsFn getCommandResultsFn;

    /** Handle to GetCommandResultName function. */
    GetCommandResultNameFn getCommandResultNameFn;

    /** Handle to GetCommandResultType function. */
    GetCommandResultUnitsTypeFn getCommandResultUnitsTypeFn;

    /** Handle to GetCommandResultUnits function. */
    GetCommandResultUnitsFn getCommandResultUnitsFn;

	/** Handle to GetEnumerations function. */
	GetEnumerationsFn getEnumerationsFn;

	/** Handle to GetEnumerationName function. */
	GetEnumerationNameFn getEnumerationNameFn;

	/** Handle to GetEnumerationValues function. */
	GetEnumerationValuesFn getEnumerationValuesFn;

	/** Handle to GetEnumerationValue function. */
	GetEnumerationValueFn getEnumerationValueFn;

    /** Handle to DoCommand function. */
    DoCommandFn doCommandFn;

    /** Handle to GetResultName function. */
    GetResultNameFn getResultNameFn;

    /** Handle to GetResult function. */
    GetResultFn getResultFn;

    /** Handle to GetDllVersion function. */
    GetDllVersionFn getDllVersionFn;
};


#endif
