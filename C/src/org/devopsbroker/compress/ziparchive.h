/*
 * ziparchive.h - DevOpsBroker C header file for the org.devopsbroker.compress.zip.ZipArchive struct
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
 * echo ORG_DEVOPSBROKER_COMPRESS_ZIPARCHIVE | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_COMPRESS_ZIP_ZIPARCHIVE_H
#define ORG_DEVOPSBROKER_COMPRESS_ZIP_ZIPARCHIVE_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>

#include <assert.h>

#include "../adt/listarray.h"
#include "../io/async.h"
#include "../io/filebuffer.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

/*
 * Zip Archive
 *    - List of FileBuffer structs
 *    - AIOFile struct for zip archive file
 *    - AIOContext for Linux AIO reads
 *    - Output directory for zip file artifacts
 */
typedef struct ZipArchive {
	AIOFile          aioFile;
	FileBufferList   bufferList;
	AIOContext      *aioContext;
	char            *outputDir;
} ZipArchive;

#if __SIZEOF_POINTER__ == 8
static_assert(sizeof(ZipArchive) == 432, "Check your assumptions");
#elif  __SIZEOF_POINTER__ == 4
static_assert(sizeof(ZipArchive) == 384, "Check your assumptions");
#endif

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce667b0d_createZipArchive
 * Description: Creates a ZipArchive struct instance
 *
 * Returns:     A ZipArchive struct instance
 * ----------------------------------------------------------------------------
 */
ZipArchive *ce667b0d_createZipArchive();

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce667b0d_destroyZipArchive
 * Description: Frees the memory allocated to the ZipArchive struct pointer
 *
 * Parameters:
 *   zipArchive	A pointer to the ZipArchive instance to destroy
 * ----------------------------------------------------------------------------
 */
void ce667b0d_destroyZipArchive(ZipArchive *zipArchive);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce667b0d_cleanUpZipArchive
 * Description: Frees dynamically allocated memory within the ZipArchive instance
 *
 * Parameters:
 *   zipArchive	A pointer to the ZipArchive instance to clean up
 * ----------------------------------------------------------------------------
 */
void ce667b0d_cleanUpZipArchive(ZipArchive *zipArchive);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce667b0d_initZipArchive
 * Description: Initializes an existing ZipArchive struct
 *
 * Parameters:
 *   zipArchive     A pointer to the ZipArchive instance to initalize
 *   aioContext     The AIOContext to use for Zip file access
 *   fileName       The name of the Zip archive file
 * ----------------------------------------------------------------------------
 */
void ce667b0d_initZipArchive(ZipArchive *zipArchive, AIOContext *aioContext, char *fileName);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce667b0d_unzip
 * Description: Extract files from a Zip archive into the current directory
 *
 * Parameters:
 *   zipArchive     The ZipArchive instance to unzip
 * ----------------------------------------------------------------------------
 */
void ce667b0d_unzip(ZipArchive *zipArchive);

#endif /* ORG_DEVOPSBROKER_COMPRESS_ZIP_ZIPARCHIVE_H */
