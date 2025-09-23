/** @file find_dll.cpp
 * Locate DLL within controller interface release/build directory structure.
 *
 * @authors Graham Bartlett
 * @copyright Prior Scientific Instruments Ltd., 2021
 */

#include "pch.h"
#include "find_dll.hpp"

#include <string>

#ifdef WIN32

#include "windows.h"
#include "shlwapi.h"

bool FindDll(std::string& dllFilename) {
	std::string dll, dllDebugSuffix, dllSizeSuffix;

#ifdef _DEBUG
	dllDebugSuffix = "d";
#else
	dllDebugSuffix = "";
#endif

	if (sizeof(void*) == 4) {
		dllSizeSuffix = "";
	}
	else {
		dllSizeSuffix = "64";
	}

	/* First look for debug/release DLL in current directory */
	dll.assign("C:\\WLI\\controller_interface");
	dll.append(dllSizeSuffix);
	//dll.append(dllDebugSuffix);
	dll.append(".dll");
	if (PathFileExistsA(dll.c_str())) {
		dllFilename.assign(dll);
		return true;
	}

#ifdef _DEBUG
	/* For debug build, try looking for release DLL in current directory */
	dll.assign("controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(".dll");
	if (PathFileExistsA(dll.c_str())) {
		dllFilename.assign(dll);
		return true;
	}
#endif

	/* Try looking for DLL in release directory structure, running
	* prebuilt binary.  This goes
	* /controller_interface/bin/<platform>/<DLL binaries>
	* /utilities/<demo>/cpp/bin/<platform>/<demo binaries>
	*
	* The release directory structure does not have a debug version.
	*/
	dll.assign("..\\..\\..\\..\\..\\controller_interface\\bin\\Windows\\controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(".dll");
	if (PathFileExistsA(dll.c_str())) {
		dllFilename.assign(dll);
		return true;
	}

	/* Try looking for DLL in release directory structure, running
	* from Visual Studio project.  This goes
	* /controller_interface/bin/<platform>/<DLL binaries>
	* /utilities/<demo>/cpp/win32/<Win32|x64>/<Debug|Release>/<demo binaries>
	*
	* The release directory structure does not have a debug version.
	*/
	dll.assign("..\\..\\..\\..\\..\\..\\controller_interface\\bin\\Windows\\controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(".dll");
	if (PathFileExistsA(dll.c_str())) {
		dllFilename.assign(dll);
		return true;
	}

	/* Try looking for DLL in local build directory structure, running the
	* executable directly.  This goes
	* /controller_interface/bin/win32/<DLL binaries>
	* /utilities/<demo>/cpp/win32/<Win32|x64>/<Debug|Release>/<demo binaries>
	*/
	dll.assign("..\\..\\..\\..\\..\\..\\controller_interface\\bin\\win32\\controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(dllDebugSuffix);
	dll.append(".dll");
	if (PathFileExistsA(dll.c_str())) {
		dllFilename.assign(dll);
		return true;
	}

#ifdef _DEBUG
	/* Again for debug build, try looking for release DLL in this directory */
	dll.assign("..\\..\\..\\..\\..\\..\\controller_interface\\bin\\win32\\controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(".dll");
	if (PathFileExistsA(dll.c_str())) {
		dllFilename.assign(dll);
		return true;
	}
#endif

	/* Try looking for DLL in local build directory structure, running
	* from Visual Studio project.  This goes
	* /controller_interface/bin/win32/<DLL binaries>
	* /utilities/<demo>/cpp/win32/<Win32|x64>/<Debug|Release>/<demo binaries>
	*/
	dll.assign("..\\..\\..\\..\\controller_interface\\bin\\win32\\controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(dllDebugSuffix);
	dll.append(".dll");
	if (PathFileExistsA(dll.c_str())) {
		dllFilename.assign(dll);
		return true;
	}

#ifdef _DEBUG
	/* Again for debug build, try looking for release DLL in this directory */
	dll.assign("..\\..\\..\\..\\controller_interface\\bin\\win32\\controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(".dll");
	if (PathFileExistsA(dll.c_str())) {
		dllFilename.assign(dll);
		return true;
	}
#endif

	/* DLL not found */
	return false;
}

#else

#include <fcntl.h>
#include <sys/stat.h>

bool FindDll(std::string& dllFilename) {
	struct stat dummyStat;
	std::string dll, dllDebugSuffix, dllSizeSuffix;

#ifdef _DEBUG
	dllDebugSuffix = "d";
#else
	dllDebugSuffix = "";
#endif

	if (sizeof(void*) == 4) {
		dllSizeSuffix = "";
	}
	else {
		dllSizeSuffix = "64";
	}

	/* First look for debug/release DLL in current directory */
	dll.assign("./controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(dllDebugSuffix);
	dll.append(".so");
	if (stat(dll.c_str(), &dummyStat) == 0) {
		dllFilename.assign(dll);
		return true;
	}

#ifdef _DEBUG
	/* For debug build, try looking for release DLL in current directory */
	dll.assign("./controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(".so");
	if (stat(dll.c_str(), &dummyStat) == 0) {
		dllFilename.assign(dll);
		return true;
	}
#endif

	/* Try looking for DLL in release directory structure.  This goes
	* /controller_interface/bin/<platform>/<DLL binaries>
	* /examples/<example>/cpp/bin/<platform>/<example binaries>
	*
	* The release directory structure does not have a debug version.
	*/
	dll.assign("../../../../../controller_interface/bin/linux/controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(".so");
	if (stat(dll.c_str(), &dummyStat) == 0) {
		dllFilename.assign(dll);
		return true;
	}

	/* Try looking for DLL in local build directory structure.  This goes
	* /controller_interface/bin/linux/<DLL binaries>
	* /examples/<example>/cpp/linux/<example binaries>
	*/
	dll.assign("../../../../controller_interface/bin/linux/controller_interface");
	dll.append(dllSizeSuffix);
	dll.append(".so");
	if (stat(dll.c_str(), &dummyStat) == 0) {
		dllFilename.assign(dll);
		return true;
	}

	/* DLL not found */
	return false;
}

#endif