/*
 * linux.h - DevOpsBroker C header file for procfs and sysfs Linux system information
 *
 * Copyright (C) 2020 Edward Smith <edwardsmith@devopsbroker.org>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------------------
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-40
 *
 * echo ORG_DEVOPSBROKER_SYSTEM_LINUX | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_SYSTEM_LINUX_H
#define ORG_DEVOPSBROKER_SYSTEM_LINUX_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>

#include <assert.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e0004944_getUnsignedInt
 * Description: Retrieves the unsigned integer at the specified path
 *
 * Parameters:
 *   infoPath   The information path value to retrieve
 * Returns:     An unsigned integer value
 * ----------------------------------------------------------------------------
 */
uint32_t e0004944_getUnsignedInt(char *infoPath);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e0004944_setUnsignedInt
 * Description: Sets the unsigned integer at the specified path
 *
 * Parameters:
 *   infoPath   The information path value to modify
 *   value      The unsigned integer value to set
 * ----------------------------------------------------------------------------
 */
void e0004944_setUnsignedInt(char *infoPath, uint32_t value);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e0004944_printError
 * Description: Prints an error message associated with the given error number
 *
 * Parameters:
 *   infoPath   The information path value with the error
 *   errorNum   The error number generated by a syscall
 * ----------------------------------------------------------------------------
 */
void e0004944_printError(char *infoPath, int errorNum);

#endif /* ORG_DEVOPSBROKER_SYSTEM_LINUX_H */
