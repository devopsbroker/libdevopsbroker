/*
 * directory.h - DevOpsBroker C header file for the org.devopsbroker.fs.Directory struct
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
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-28
 *
 * echo ORG_DEVOPSBROKER_FS_DIRECTORY | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_FS_DIRECTORY_H
#define ORG_DEVOPSBROKER_FS_DIRECTORY_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>

#include <assert.h>

#include "../adt/listarray.h"
#include "../lang/stringbuilder.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef StringBuilder DirPath;

static_assert(sizeof(DirPath) == 16, "Check your assumptions");

typedef struct File {
	char name[256];
} File;

static_assert(sizeof(File) == 256, "Check your assumptions");

typedef struct Directory {
	char name[256];
	ListArray subdirList;
	ListArray fileList;
} Directory;

static_assert(sizeof(Directory) == 288, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_createDirectory
 * Description: Creates a Directory struct instance
 *
 * Parameters:
 *   name       A pointer to the name of the directory
 * Returns:     A Directory struct instance
 * ----------------------------------------------------------------------------
 */
Directory *d0059b5b_createDirectory(char *name);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_createDirPath
 * Description: Creates a DirPath struct instance
 *
 * Parameters:
 *   name       A pointer to the name of the directory path
 * Returns:     A DirPath struct instance
 * ----------------------------------------------------------------------------
 */
DirPath *d0059b5b_createDirPath(char *path);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_createFile
 * Description: Creates a File struct instance
 *
 * Parameters:
 *   name       A pointer to the name of the file
 * Returns:     A File struct instance
 * ----------------------------------------------------------------------------
 */
File *d0059b5b_createFile(char *name);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_destroyDirectory
 * Description: Frees the memory allocated to the Directory struct pointer
 *
 * Parameters:
 *   directory	A pointer to the Directory instance to destroy
 * ----------------------------------------------------------------------------
 */
void d0059b5b_destroyDirectory(Directory *directory);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_destroyDirPath
 * Description: Frees the memory allocated to the DirPath struct pointer
 *
 * Parameters:
 *   dirPath    A pointer to the DirPath instance to destroy
 * ----------------------------------------------------------------------------
 */
void d0059b5b_destroyDirPath(DirPath *dirPath);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_destroyFile
 * Description: Frees the memory allocated to the File struct pointer
 *
 * Parameters:
 *   file   A pointer to the File instance to destroy
 * ----------------------------------------------------------------------------
 */
void d0059b5b_destroyFile(File *file);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_cleanUpDirectory
 * Description: Frees dynamically allocated memory within the Directory instance
 *
 * Parameters:
 *   directory	A pointer to the Directory instance to clean up
 * ----------------------------------------------------------------------------
 */
void d0059b5b_cleanUpDirectory(Directory *directory);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_cleanUpDirPath
 * Description: Frees dynamically allocated memory within the DirPath instance
 *
 * Parameters:
 *   dirPath    A pointer to the DirPath instance to clean up
 * ----------------------------------------------------------------------------
 */
void d0059b5b_cleanUpDirPath(DirPath *dirPath);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_initDirectory
 * Description: Initializes an existing Directory struct
 *
 * Parameters:
 *   directory  A pointer to the Directory instance to initalize
 *   name       A pointer to the name of the directory
 * ----------------------------------------------------------------------------
 */
void d0059b5b_initDirectory(Directory *directory, char *name);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_initDirPath
 * Description: Initializes an existing DirPath struct
 *
 * Parameters:
 *   dirPath    A pointer to the DirPath instance to initalize
 *   path       A pointer to the name of the directory path
 * ----------------------------------------------------------------------------
 */
void d0059b5b_initDirPath(DirPath *dirPath, char *path);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d0059b5b_listContents
 * Description: Lists the contents of the specified Directory
 *
 * Parameters:
 *   directory      A pointer to the Directory instance to list
 *   dirPath        A pointer of the DirPath to list
 *   isRecursive    True if list directory contents recursively, false otherwise
 * ----------------------------------------------------------------------------
 */
void d0059b5b_listContents(Directory *directory, DirPath *dirPath, bool isRecursive);

#endif /* ORG_DEVOPSBROKER_FS_DIRECTORY_H */
