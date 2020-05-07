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

#define _GNU_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "directory.h"

#include "../io/file.h"
#include "../lang/error.h"
#include "../lang/memory.h"
#include "../lang/string.h"
#include "../lang/stringbuilder.h"
#include "../terminal/ansi.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define DIRPATH_BUFFER_SIZE 64
#define FILEPATHLIST_SIZE 64

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static void addSubdir(Directory *directory, Directory *subdir);
static void addFile(Directory *directory, File *file);

static int compareDirectory(void *first, void *second);
static int compareFile(void *first, void *second);

static void readDirectory(Directory *directory, DirPath *dirPath);

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

Directory *d0059b5b_createDirectory(char *name) {
	Directory *directory = f668c4bd_malloc(sizeof(Directory));

	// Copy directory name into directory->name
	f6215943_copy(name, directory->name);

	// Lazy init the subdirectory list and file list later
	f668c4bd_meminit(&directory->subdirList, sizeof(ListArray) * 2);

	return directory;
}

DirPath *d0059b5b_createDirPath(char *path) {
	DirPath *dirPath = f668c4bd_malloc(sizeof(DirPath));

	dirPath->length = f6215943_getLength(path);
	dirPath->size = dirPath->length + DIRPATH_BUFFER_SIZE;
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

FilePathList *d0059b5b_createFilePathList() {
	FilePathList *filePathList = f668c4bd_malloc(sizeof(FilePathList));

	filePathList->values = f668c4bd_mallocArray(sizeof(void*), FILEPATHLIST_SIZE);
	filePathList->size = FILEPATHLIST_SIZE;
	filePathList->length = 0;

	return filePathList;
}

void d0059b5b_destroyDirectory(Directory *directory) {
	// Clean up the subdirectory list
	for (int i=0; i < directory->subdirList.length; i++) {
		d0059b5b_destroyDirectory(directory->subdirList.values[i]);
	}
	b196167f_cleanUpListArray(&directory->subdirList, NULL);

	// Clean up the file list
	b196167f_cleanUpListArray(&directory->fileList, f668c4bd_free);

	// Free the Directory struct
	f668c4bd_free(directory);
}

void d0059b5b_destroyDirPath(DirPath *dirPath) {
	f668c4bd_free(dirPath->buffer);
	f668c4bd_free(dirPath);
}

void d0059b5b_destroyFilePathList(FilePathList *filePathList) {
	b196167f_destroyListArray(filePathList, f668c4bd_free);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void d0059b5b_cleanUpDirectory(Directory *directory) {
	// Clean up the subdirectory list
	for (int i=0; i < directory->subdirList.length; i++) {
		d0059b5b_destroyDirectory(directory->subdirList.values[i]);
	}
	b196167f_cleanUpListArray(&directory->subdirList, NULL);

	// Clean up the file list
	b196167f_cleanUpListArray(&directory->fileList, f668c4bd_free);
}

void d0059b5b_cleanUpDirPath(DirPath *dirPath) {
	f668c4bd_free(dirPath->buffer);
}

void d0059b5b_cleanUpFilePathList(FilePathList *filePathList) {
	f668c4bd_free(filePathList->values);
}

void d0059b5b_initDirectory(Directory *directory, char *name) {
	// Copy directory name into directory->name
	f6215943_copy(name, directory->name);

	// Lazy init the subdirectory list and file list later
	f668c4bd_meminit(&directory->subdirList, sizeof(ListArray) * 2);
}

void d0059b5b_initDirPath(DirPath *dirPath, char *path) {
	dirPath->length = f6215943_getLength(path);
	dirPath->size = dirPath->length + DIRPATH_BUFFER_SIZE;
	dirPath->buffer = f668c4bd_mallocArray(sizeof(char), dirPath->size);
	f6215943_copyToBuffer(path, dirPath->buffer, dirPath->size);
}

void d0059b5b_initFilePathList(FilePathList *filePathList) {
	filePathList->values = f668c4bd_mallocArray(sizeof(void*), FILEPATHLIST_SIZE);
	filePathList->size = FILEPATHLIST_SIZE;
	filePathList->length = 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void d0059b5b_listContents(Directory *directory, DirPath *dirPath, bool isRecursive) {
	// 1. Read the contents of the directory
	readDirectory(directory, dirPath);

	// 2. Search recursively, if necessary
	c598a24c_append_char(dirPath, '/');
	uint32_t dirPathLen = dirPath->length;

	if (isRecursive && directory->subdirList.length > 0) {
		Directory *subdir;

		for (uint32_t i=0; i < directory->subdirList.length; i++) {
			subdir = directory->subdirList.values[i];

			c598a24c_append_string(dirPath, subdir->name);
			d0059b5b_listContents(subdir, dirPath, isRecursive);
			c598a24c_reduceLength(dirPath, dirPathLen);
		}
	}

	c598a24c_reduceLength(dirPath, dirPathLen - 1);
}

bool d0059b5b_makeDirectory(char *pathName, uint32_t mode, bool hasFilename) {
	FileStatus dirStatus;
	ListArray subdirList;
	char *subdir;
	bool makeDir;

	// 1. Initialize data
	b196167f_initListArray(&subdirList);

	f6215943_splitWithChar(pathName, '/', &subdirList);

	subdir = pathName;
	makeDir = false;

	if (subdir[0] == '\0') {
		subdir[0] = '/';
		subdirList.length--;
	}

	// 2. Create subdirectories from the base directory to the leaves
	for (uint32_t i=1; i <= subdirList.length; i++) {
		if (i < subdirList.length || !hasFilename) {
			if (stat(pathName, &dirStatus) == SYSTEM_ERROR_CODE && errno == ENOENT) {
				if (mkdir(pathName, mode) == SYSTEM_ERROR_CODE) {
					StringBuilder errorMessage;

					c598a24c_initStringBuilder(&errorMessage);

					c598a24c_append_string(&errorMessage, "Cannot make directory '");
					c598a24c_append_string(&errorMessage, pathName);
					c598a24c_append_char(&errorMessage, '\'');

					c7c88e52_printLibError(errorMessage.buffer, errno);
					c598a24c_cleanUpStringBuilder(&errorMessage);
				} else {
					makeDir = true;
				}
			}
		}

		if (i < subdirList.length) {
			// Advance to the next subdirectory and put back the '/'
			while ((*subdir) != '\0') {
				subdir++;
			}

			(*subdir) = '/';
		}
	}

	return makeDir;
}

void d0059b5b_find(FilePathList *filePathList, DirPath *dirPath, bool isMatch(char *filename)) {
	Directory currentDir;

	// 1. Read the contents of the directory
	d0059b5b_initDirectory(&currentDir, dirPath->buffer);
	readDirectory(&currentDir, dirPath);

	c598a24c_append_char(dirPath, '/');
	uint32_t dirPathLen = dirPath->length;

	// 2. Search recursively, if necessary
	if (currentDir.subdirList.length > 0) {
		Directory *subdir;

		for (uint32_t i=0; i < currentDir.subdirList.length; i++) {
			subdir = currentDir.subdirList.values[i];

			c598a24c_append_string(dirPath, subdir->name);
			d0059b5b_find(filePathList, dirPath, isMatch);
			c598a24c_reduceLength(dirPath, dirPathLen);
		}
	}

	// 3. Find files in the current directory that match the expression
	if (currentDir.fileList.length > 0) {
		File *file;
		char *filePath;

		for (uint32_t i=0; i < currentDir.fileList.length; i++) {
			file = currentDir.fileList.values[i];

			if (isMatch(file->name)) {
				filePath = c598a24c_createString(dirPath, file->name);

				// Lazy init the FilePathList
				if (filePathList == NULL) {
					filePathList = d0059b5b_createFilePathList();
				}

				b196167f_add(filePathList, filePath);
			}
		}
	}

	c598a24c_reduceLength(dirPath, dirPathLen - 1);
	d0059b5b_cleanUpDirectory(&currentDir);
}

void d0059b5b_printDirectory(Directory *directory, DirPath *dirPath) {
	Directory *subdir;
	File *file;
	uint32_t i, dirPathLen;

	printf("%s:\n", dirPath->buffer);
	printf(ANSI_BLUE);
	for (i=0; i < directory->subdirList.length; i++) {
		subdir = directory->subdirList.values[i];
		puts(subdir->name);
	}

	printf(ANSI_RESET);
	for (i=0; i < directory->fileList.length; i++) {
		file = directory->fileList.values[i];
		puts(file->name);
	}

	printf("\n");

	if (directory->subdirList.length > 0) {
		dirPathLen = dirPath->length + 1;

		c598a24c_append_char(dirPath, '/');
		for (i=0; i < directory->subdirList.length; i++) {
			subdir = directory->subdirList.values[i];

			c598a24c_append_string(dirPath, subdir->name);
			d0059b5b_printDirectory(subdir, dirPath);
			c598a24c_reduceLength(dirPath, dirPathLen);
		}
	}
}

void d0059b5b_printFilePathList(FilePathList *filePathList) {
	for (uint32_t i=0; i < filePathList->length; i++) {
		puts(filePathList->values[i]);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Static Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static void addSubdir(Directory *directory, Directory *subdir) {
	// Lazy init the subdirectory list, if necessary
	if (directory->subdirList.values == NULL) {
		b196167f_initListArray(&directory->subdirList);
	}

	b196167f_add(&directory->subdirList, subdir);
}

static void addFile(Directory *directory, File *file) {
	// Lazy init the file list, if necessary
	if (directory->fileList.values == NULL) {
		b196167f_initListArray(&directory->fileList);
	}

	b196167f_add(&directory->fileList, file);
}

static int compareDirectory(void *first, void *second) {
	return f6215943_compare(((Directory *)first)->name, ((Directory *)second)->name);
}

static int compareFile(void *first, void *second) {
	return f6215943_compare(((File *)first)->name, ((File *)second)->name);
}

static void readDirectory(Directory *directory, DirPath *dirPath) {
	DIR *dirStream;
	struct dirent *dirEntry;
	Directory *subdir;
	File *file;
	errno = 0;

	// 1. Open directory
	dirStream = opendir(dirPath->buffer);

	// 2. Read the contents of the directory
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
					subdir = d0059b5b_createDirectory(dirEntry->d_name);
					addSubdir(directory, subdir);
				}
			} else if (dirEntry->d_type == DT_REG) {
				file = d0059b5b_createFile(dirEntry->d_name);
				addFile(directory, file);
			}
		} while(true);

		if (closedir(dirStream) == SYSTEM_ERROR_CODE) {
			c7c88e52_printLibError("Cannot close directory stream", errno);
			abort();
		}

		// 3. Sort the subdirectory list and file list
		b196167f_sort(&directory->subdirList, compareDirectory);
		b196167f_sort(&directory->fileList, compareFile);
	}
}
