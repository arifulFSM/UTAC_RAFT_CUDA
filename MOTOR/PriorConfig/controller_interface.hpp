/**
@file controller_interface.hpp
Controller interface C++-style API.

@authors Graham Bartlett
@copyright Prior Scientific Instruments Ltd., 2021
*/

#ifndef CONTROLLER_INTERFACE_HPP
#define CONTROLLER_INTERFACE_HPP

#include <string>
#include <list>

#include "controller_interface.h"


/** Controller interface C++-style API.
*/
class ControllerInterface
{
public:
    /** Constructor.
     */
    ControllerInterface();

    /** Destructor.
     *
     * Note that this is not virtual because we do not expect anyone to inherit
     * from this class.
     */
    ~ControllerInterface();

    /** Get controllers currently connected to host PC.
    * @param devices List of URIs to controllers currently connected
    * @returns SUCCESS if OK
    */
    ErrorCode GetDevices(std::list<std::string>& devices);

    /** Attempt to open a session to a controller on the specified device.
    * @param device Controller URI
    * @returns SUCCESS if OK
    */
    ErrorCode OpenSession(const std::string& device);

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
	 * @returns SUCCESS if OK
	 */
	ErrorCode OpenSessionDatalink(ControllerInterfaceDatalinkReadFn datalinkRead,
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
    * @returns SUCCESS if OK
    */
    ErrorCode FindCommands(std::list<std::string>& commands, const std::string& filter);

    /** Get command description.
    * @param commandName Command name
    * @param commandDescription Command description
    * @returns SUCCESS if OK
    */
    ErrorCode GetCommandDescription(const std::string& commandName, std::string& description);

    /** Get command parameters and units for each parameter.
    * @param commandName Command name
    * @param channel Channel index
    * @param commandParameterNames Command parameter names
    * @param commandParameterUnitTypes Command parameter unit types
    * @param commandParameterUnits Command parameter units
    * @returns SUCCESS if OK
    */
    ErrorCode GetCommandParameters(const std::string& commandName, const int channel, std::list<std::string>& commandParameterNames, std::list<std::string>& commandParameterUnitTypes, std::list<std::string>& commandParameterUnits);

    /** Get command results and units for each parameter.
    * @param commandName Command name
    * @param channel Channel index
    * @param commandResultNames Command result names
    * @param commandResultUnitTypes Command result unit types
    * @param commandResultUnits Command result units
    * @returns SUCCESS if OK
    */
    ErrorCode GetCommandResults(const std::string& commandName, const int channel, std::list<std::string>& commandResultNames, std::list<std::string>& commandResultUnitTypes, std::list<std::string>& commandResultUnits);

	/** Get enumerations available on the connected controller.
    * @param enumerations List of enumerations
    * @returns SUCCESS if OK
    */
	ErrorCode GetEnumerationNames(std::list<std::string>& enumerations);

	/** Get values available for an enumeration.
    * @param enumerationName Enumeration name
    * @param enumerationValues Enumeration values available
    * @returns SUCCESS if OK
    */
	ErrorCode GetEnumerationValues(const std::string& enumerationName, std::list<std::string>& enumerationValues);
	
    /** Send command(s) to controller.
    * @param command Command to send
    * @param resultNames Result names
    * @param resultValues Result values
    * @param isUnknown If command fails, notify user that this is due to command unknown and not other causes
    * @param isIncorrectParams If commands fails, notify user that this is due to incorrect parameters and not other causes
    * @returns SUCCESS if OK
    */
    ErrorCode DoCommand(const std::string& command, std::list<std::string>& resultNames, std::list<std::string>& resultValues);

	/** For testing/debugging, force the DLL to use the specified security level,
	* so that all commands with that security level are visible.
	*
	* Note that this does not grant access to those commands.  The DLL can issue
	* those commands, but the controller will fail with a security error.
	*
	* Also note that any command which reads the actual security level from the
	* controller will return the DLL back to the actual controller security level.
	*
	* @param level Security level (see code for values)
	*/
	void ForceSecurityLevel(const int level);

private:
    /** Explicitly prohibit copy constructor. */
    ControllerInterface(ControllerInterface& copy);

	/** Internal data for controller interface */
	void* interfaceData;
};


#endif
