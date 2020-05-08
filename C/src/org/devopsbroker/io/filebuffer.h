/*
 * filebuffer.h - DevOpsBroker C header file for the org.devopsbroker.io.FileBuffer struct
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
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-46
 *
 * echo ORG_DEVOPSBROKER_IO_FILEBUFFER | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_IO_FILEBUFFER_H
#define ORG_DEVOPSBROKER_IO_FILEBUFFER_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>
#include <stdbool.h>

#include <assert.h>

#include "../io/async.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct FileBuffer {
	void              *buffer;
	struct FileBuffer *next;
	int64_t            fileOffset;
	uint32_t           dataOffset;
	uint32_t           numBytes;
} FileBuffer;

static_assert(sizeof(FileBuffer) == 32, "Check your assumptions");

typedef struct FileBufferList {
	FileBuffer **values;
	uint64_t     numBytes;
	int64_t      fileOffset;
	uint32_t     length;
	uint32_t     size;
} FileBufferList;

static_assert(sizeof(FileBufferList) == 32, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_createFileBuffer
 * Description: Creates a FileBuffer struct instance
 *
 * Parameters:
 *   buffer     A pointer to the underlying buffer for the file
 * Returns:     A FileBuffer struct instance
 * ----------------------------------------------------------------------------
 */
FileBuffer *ce97d170_createFileBuffer(void *buffer);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_destroyFileBuffer
 * Description: Frees the memory allocated to the FileBuffer struct pointer
 *
 * Parameters:
 *   fileBuffer    A pointer to the FileBuffer instance to destroy
 * ----------------------------------------------------------------------------
 */
void ce97d170_destroyFileBuffer(FileBuffer *fileBuffer);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_createFileBufferList
 * Description: Creates a FileBufferList struct instance
 *
 * Returns:     A FileBufferList struct instance
 * ----------------------------------------------------------------------------
 */
FileBufferList *ce97d170_createFileBufferList();

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_destroyFileBufferList
 * Description: Frees the memory allocated to the FileBufferList struct pointer
 *
 * Parameters:
 *   bufferList     A pointer to the FileBufferList instance to destroy
 *   freeBuffer     A function pointer to the method that frees the underlying
 *                  buffer contained within the FileBuffer instance
 * ----------------------------------------------------------------------------
 */
void ce97d170_destroyFileBufferList(FileBufferList *bufferList, void freeBuffer(void *buffer));

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_initFileBuffer
 * Description: Initializes an existing FileBuffer struct
 *
 * Parameters:
 *   fileBuffer     A pointer to the FileBuffer instance to initalize
 *   buffer         A pointer to the underlying buffer for the file
 * ----------------------------------------------------------------------------
 */
void ce97d170_initFileBuffer(FileBuffer *fileBuffer, void *buffer);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_cleanUpFileBufferList
 * Description: Frees dynamically allocated memory within the FileBufferList instance
 *
 * Parameters:
 *   bufferList     A pointer to the FileBufferList instance to clean up
 *   freeBuffer     A function pointer to the method that frees the underlying
 *                  buffer contained within the FileBuffer instance
 * ----------------------------------------------------------------------------
 */
void ce97d170_cleanUpFileBufferList(FileBufferList *bufferList, void freeBuffer(void *buffer));

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_initFileBufferList
 * Description: Initializes an existing FileBufferList struct
 *
 * Parameters:
 *   bufferList     A pointer to the FileBufferList instance to initalize
 * ----------------------------------------------------------------------------
 */
void ce97d170_initFileBufferList(FileBufferList *bufferList);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_resetFileBufferList
 * Description: Resets the FileBufferList instance by freeing all dynamically
 *              allocated memory and resetting to its intitial state
 *
 * Parameters:
 *   bufferList     A pointer to the FileBufferList instance to reset
 *   freeBuffer     A function pointer to the method that frees the underlying
 *                  buffer contained within the FileBuffer instance
 * ----------------------------------------------------------------------------
 */
void ce97d170_resetFileBufferList(FileBufferList *bufferList, void freeBuffer(void *buffer));

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_addBuffer
 * Description: Returns true if the FileBuffer contains the data, false otherwise
 *
 * Parameters:
 *   bufferList     A pointer to the FileBufferList instance
 *   fileBuffer     A pointer to the FileBuffer instance to add
 * ----------------------------------------------------------------------------
 */
void ce97d170_addBuffer(FileBufferList *bufferList, FileBuffer *fileBuffer);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_containsData
 * Description: Returns true if the FileBuffer contains the data, false otherwise
 *
 * Parameters:
 *   bufferList     A pointer to the FileBufferList instance to inspect
 *   offset         The file offset where the data begins
 *   length         The length of the data starting from the offset
 * Returns:     The FileBuffer that contains the beginning of the data, or NULL
 *              otherwise
 * ----------------------------------------------------------------------------
 */
FileBuffer *ce97d170_containsData(FileBufferList *bufferList, int64_t offset, uint32_t length);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_crc32
 * Description: Calculates the CRC-32 of the FileBuffer for length bytes
 *
 * Parameters:
 *   fileBuffer     A pointer to the FileBuffer instance to begin with
 *   length         The length of the buffer to calculate
 * Returns:     The calculated CRC-32 of the buffer
 * ----------------------------------------------------------------------------
 */
uint32_t ce97d170_crc32(FileBuffer *fileBuffer, uint32_t length);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_getBuffer
 * Description: Returns the FileBuffer at the specified index
 *
 * Parameters:
 *   bufferList     A pointer to the FileBufferList instance
 *   index          The index of the FileBuffer to retrieve
 * Returns:     The FileBuffer at the specified index, or NULL if invalid index
 * ----------------------------------------------------------------------------
 */
FileBuffer *ce97d170_getBuffer(FileBufferList *bufferList, uint32_t index);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_readFileBuffer
 * Description: Returns a FileBuffer instance populated with the requested data
 *
 * Parameters:
 *   aioFile    A pointer to the AIOFile instance
 *   length     The length of data to read
 * Returns:     A FileBuffer instance populated with the requested data
 * ----------------------------------------------------------------------------
 */
FileBuffer *ce97d170_readFileBuffer(AIOFile *aioFile, uint32_t length);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_readFileBufferList
 * Description: Loads the requested data into the FileBufferList instance
 *
 * Parameters:
 *   aioFile        A pointer to the AIOFile instance
 *   bufferList     A pointer to the FileBufferList instance to populate
 *   length         The length of data to read
 * ----------------------------------------------------------------------------
 */
void ce97d170_readFileBufferList(AIOFile *aioFile, FileBufferList *bufferList, uint32_t length);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_write
 * Description: Writes a FileBuffer instance out to the specified AIOFile
 *
 * Parameters:
 *   fileBuffer     A pointer to the FileBuffer instance to begin with
 *   fd             The file descriptor to write to
 *   length         The data length to write to the file
 *   pathName       The name of the file to write to (used for error handling)
 * ----------------------------------------------------------------------------
 */
void ce97d170_write(FileBuffer *fileBuffer, int fd, uint32_t length, char *pathName);

#endif /* ORG_DEVOPSBROKER_IO_FILEBUFFER_H */
