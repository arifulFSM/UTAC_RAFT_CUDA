/** @file find_dll.hpp
 *
 * Locate DLL within controller interface release/build directory structure.
 *
 * @authors Graham Bartlett
 * @copyright Prior Scientific Instruments Ltd., 2021
 */

#include "pch.h"

#ifndef FIND_DLL_HPP
#define FIND_DLL_HPP

#include <string>

 /** Locate DLL within controller interface release/build directory structure.
 * @param dllFilename DLL filename found
 * @returns True if found
 */
extern bool FindDll(std::string& dllFilename);

#endif