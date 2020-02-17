/*
 * directory.c - DevOpsBroker C source file for the org.devopsbroker.fs.Directory struct
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
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include "directory.h"

#include "../lang/error.h"
#include "../lang/string.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define DIRPATH_BUFFER 64

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static int compareDirectory(void *first, void *second);
static int compareFile(void *first, void *second);

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

Directory *d0059b5b_createDirectory(char *name) {
	Directory *directory = f668c4bd_malloc(sizeof(Directory));

	// Copy directory name into directory->name
	f6215943_copy(name, directory->name);

	// Initialize the subdirectory list and file list
	b196167f_initListArray(&directory->subdirList);
	b196167f_initListArray(&directory->fileList);

	return directory;
}

DirPath *d0059b5b_createDirPath(char *path) {
	DirPath *dirPath = f668c4bd_malloc(sizeof(DirPath));

	dirPath->length = f6215943_getLength(path);
	dirPath->size = dirPath->length + DIRPATH_BUFFER;
	dirPath->buffer = f668c4bd_mallocArray(sizeof(char), dirPath->size);
	f6215943_copyToBuffer(path, dirPath->buffer, dirPath->size);

	return dirPath;
}

File *d0059b5b_createFile(char *name) {
	File *file = f668c4bd_malloc(sizeof(File));

	// Copy filename into file->name
	f6215943_copy(name, file->name);

	return file;
}

void d0059b5b_destroyDirectory(Directory *directory) {
	// Clean up the subdirectory list
	for (int i=0; i < directory->subdirList.length; i++) {
		d0059b5b_destroyDirectory(directory->subdirList.values[i]);
	}
	b196167f_cleanUpListArray(&directory->subdirList);

	// Clean up the file list
	for (int i=0; i < directory->fileList.length; i++) {
		d0059b5b_destroyFile(directory->fileList.values[i]);
	}
	b196167f_cleanUpListArray(&directory->fileList);

	// Free the Directory struct
	free(directory);
}

void d0059b5b_destroyDirPath(DirPath *dirPath) {
	free(dirPath->buffer);
	free(dirPath);
}

void d0059b5b_destroyFile(File *file) {
	// Free the File struct
	free(file);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void d0059b5b_cleanUpDirectory(Directory *directory) {
	// Clean up the subdirectory list
	for (int i=0; i < directory->subdirList.length; i++) {
		d0059b5b_destroyDirectory(directory->subdirList.values[i]);
	}
	b196167f_cleanUpListArray(&directory->subdirList);

	// Clean up the file list
	for (int i=0; i < directory->fileList.length; i++) {
		d0059b5b_destroyFile(directory->fileList.values[i]);
	}
	b196167f_cleanUpListArray(&directory->fileList);
}

void d0059b5b_cleanUpDirPath(DirPath *dirPath) {
	free(dirPath->buffer);
}

void d0059b5b_initDirectory(Directory *directory, char *name) {
	// Copy directory name into directory->name
	f6215943_copy(name, directory->name);

	// Initialize the subdirectory list and file list
	b196167f_initListArray(&directory->subdirList);
	b196167f_initListArray(&directory->fileList);
}

void d0059b5b_initDirPath(DirPath *dirPath, char *path) {
	dirPath->length = f6215943_getLength(path);
	dirPath->size = dirPath->length + DIRPATH_BUFFER;
	dirPath->buffer = f668c4bd_mallocArray(sizeof(char), dirPath->size);
	f6215943_copyToBuffer(path, dirPath->buffer, dirPath->size);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void d0059b5b_listContents(Directory *directory, DirPath *dirPath, bool isRecursive) {
	DIR *dirStream;
	struct dirent *dirEntry;

	// 1. Open directory
	dirStream = opendir(dirPath->buffer);

	if (dirStream == NULL) {
		StringBuilder errorMessage;
		c598a24c_initStringBuilder(&errorMessage);

		c598a24c_append_string(&errorMessage, "Cannot open directory stream for path '");
		c598a24c_append_string(&errorMessage, dirPath->buffer);
		c598a24c_append_char(&errorMessage, '\'');

		c7c88e52_printLibError(errorMessage.buffer, errno);
		c598a24c_destroyStringBuilder(&errorMessage);
		abort();
	} else {
		// 2. Read the contents of the directory
		Directory *subDir;
		File *file;
		errno = 0;

		do {
			dirEntry = readdir(dirStream);

			if (dirEntry == NULL) {
				if (errno == 0) {
					break;
				}

				c7c88e52_printLibError("Cannot read directory entry", errno);
				abort();
			}

			// TODO: Handle symbolic links properly
			if (dirEntry->d_type == DT_DIR) {
				if ((dirEntry->d_name[0] == '.' && dirEntry->d_name[1] == '\0')
					|| (dirEntry->d_name[0] == '.' && dirEntry->d_name[1] == '.' && dirEntry->d_name[2] == '\0')) {
					// Skip the current directory and previous directory
				} else {
					subDir = d0059b5b_createDirectory(dirEntry->d_name);
					b196167f_add(&directory->subdirList, subDir);
				}
			} else if (dirEntry->d_type == DT_REG) {
				file = d0059b5b_createFile(dirEntry->d_name);
				b196167f_add(&directory->fileList, file);
			}
		} while(true);

		if (closedir(dirStream) == SYSTEM_ERROR_CODE) {
			c7c88e52_printLibError("Cannot close directory stream", errno);
			abort();
		}

		// 3. Sort the subdirectory list and file list
		b196167f_sort(&directory->subdirList, compareDirectory);
		b196167f_sort(&directory->fileList, compareFile);

		// 4. Search recursively, if necessary
		if (isRecursive && directory->subdirList.length > 0) {
			Directory *subdir;
			uint32_t dirPathLen = dirPath->length + 1;

			c598a24c_append_char(dirPath, '/');
			for (uint32_t i=0; i < directory->subdirList.length; i++) {
				subdir = directory->subdirList.values[i];

				c598a24c_append_string(dirPath, subdir->name);
				d0059b5b_listContents(subdir, dirPath, isRecursive);
				c598a24c_reduceLength(dirPath, dirPathLen);
			}
		}
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Static Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static int compareDirectory(void *first, void *second) {
	return f6215943_compare(((Directory *)first)->name, ((Directory *)second)->name);
}

static int compareFile(void *first, void *second) {
	return f6215943_compare(((File *)first)->name, ((File *)second)->name);
}
