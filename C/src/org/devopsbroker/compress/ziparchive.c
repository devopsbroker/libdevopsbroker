/*
 * ziparchive.c - DevOpsBroker C source file for the org.devopsbroker.compress.zip.ZipArchive struct
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
 * Three things to consider:
 *    1. Reading the entire file into memory
 *    2. Having the data in memory but cannot retrieve record due to a "cache miss"
 *    3. Not reading the entire file into memory
 *
 * Let's do the following:
 *    1. Read in the last page buffer to search for the EndOfCDR record
 *    2. If the CentralDirectory is in the last page buffer, process
 *       Otherwise load the entire CentralDirectory into memory
 *    3. Start processing FileHandler records by reading in first 32KB minus
 *       the size of the CentralDirectory and EndOfCDR records
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _GNU_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>
#include <stdbool.h>

#include "ziparchive.h"

#include "../fs/directory.h"
#include "../io/file.h"
#include "../io/filebuffer.h"
#include "../lang/integer.h"
#include "../lang/memory.h"
#include "../lang/string.h"
#include "../memory/pagepool.h"
#include "../time/time.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define ZIP_FILE_HEADER_SIG   0x02014b50
#define ZIP_FILE_HEADER_SIZE  46

#define ZIP_FILE_LOCAL_HEADER_SIG   0x04034b50
#define ZIP_FILE_LOCAL_HEADER_SIZE  30

#define ZIP_END_OF_CDR_SIG   0x06054b50
#define ZIP_END_OF_CDR_SIZE  22

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static bool findEndOfCDR(ZipArchive *zipArchive);
static CentralDirectory *loadCentralDirectory(ZipArchive *zipArchive);
static void processFileHeaderList(ZipArchive *zipArchive, CentralDirectory *centralDir);

static void printEndOfCDR(EndOfCDR *endOfCDR);
static void printCentralDirectory(CentralDirectory *centralDir);

static FileHeader *ce667b0d_createFileHeader();
static LocalFileHeader *ce667b0d_createLocalFileHeader();

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

ZipArchive *ce667b0d_createZipArchive() {
	ZipArchive *zipArchive = malloc(sizeof(ZipArchive));

	// TODO: Fill in with struct initialization code

	return zipArchive;
}

void ce667b0d_destroyZipArchive(ZipArchive *zipArchive) {
	f668c4bd_free(zipArchive);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void ce667b0d_cleanUpZipArchive(ZipArchive *zipArchive) {
	// 1. Close the file
	f1207515_cleanUpAIOFile(&zipArchive->aioFile);

	// 2. Clean up the ListArray declarations
	ce97d170_cleanUpFileBufferList(&zipArchive->bufferList, f502a409_releasePage);
	b196167f_cleanUpListArray(&zipArchive->centralDirectory.fileHeaderList, f668c4bd_free);
}

void ce667b0d_initZipArchive(ZipArchive *zipArchive, AIOContext *aioContext, char *fileName) {
	FileStatus fileStatus;

	// 1. Initialize all of the ZipArchive memory
	f668c4bd_meminit(zipArchive, sizeof(ZipArchive));
	ce97d170_initFileBufferList(&zipArchive->bufferList);
	b196167f_initListArray(&zipArchive->centralDirectory.fileHeaderList);

	// 2. Initialize the AIOFile struct
	f1207515_initAIOFile(&zipArchive->aioFile, aioContext, fileName);

	// 2. Open the file
	f1207515_open(&zipArchive->aioFile, FOPEN_READONLY, 0);

	// 3. Retrieve the file size
	e2f74138_getDescriptorStatus(zipArchive->aioFile.fd, &fileStatus);
	zipArchive->aioFile.fileSize = fileStatus.st_size;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ce667b0d_unzip(ZipArchive *zipArchive) {
	CentralDirectory *centralDir;

	// 2. Find the End of Central Directory Record
	if (findEndOfCDR(zipArchive)) {
		printEndOfCDR(&zipArchive->endOfCDR);

		// 3. Load the Central Directory
		centralDir = loadCentralDirectory(zipArchive);

		printCentralDirectory(centralDir);

		// 4. Load up to the first 32KB of the file
		if (zipArchive->bufferList.fileOffset > 0) {
			uint32_t dataLength;

			// Release all currently held FileBuffers
			ce97d170_resetFileBufferList(&zipArchive->bufferList, f502a409_releasePage);

			zipArchive->aioFile.offset = 0;
			if (zipArchive->aioFile.fileSize > ASYNC_AIOTICKET_MAXSIZE) {
				dataLength = ASYNC_AIOTICKET_MAXSIZE;
			} else {
				dataLength = zipArchive->aioFile.fileSize;
			}

			ce97d170_readFileBufferList(&zipArchive->aioFile, &zipArchive->bufferList, dataLength);
		}

		// 5. Process the FileHeader list obtained from the Central Directory
		processFileHeaderList(zipArchive, centralDir);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static bool findEndOfCDR(ZipArchive *zipArchive) {
	FileBuffer *fileBuffer;
	AIOFile *aioFile;
	uint32_t length;
	void *bufPtr;

	// 1. Set the initial offset
	aioFile = &zipArchive->aioFile;
	if (aioFile->fileSize > PAGEPOOL_PAGE_SIZE) {
		aioFile->offset = aioFile->fileSize - PAGEPOOL_PAGE_SIZE;
		aioFile->offset = ((aioFile->offset + 511) >> 9) << 9;
	}

	length = aioFile->fileSize - aioFile->offset;

	// 2. Read end of Zip archive
	fileBuffer = ce97d170_readFileBuffer(aioFile, length);
	ce97d170_addBuffer(&zipArchive->bufferList, fileBuffer);

	bufPtr = fileBuffer->buffer + fileBuffer->numBytes;

	if (fileBuffer->numBytes > ZIP_END_OF_CDR_SIZE) {
		for (bufPtr -= ZIP_END_OF_CDR_SIZE; bufPtr >= fileBuffer->buffer; bufPtr--) {
			if ( (*(uint32_t*)bufPtr) == ZIP_END_OF_CDR_SIG) {
				zipArchive->endOfCDR.signature = (*(uint32_t*)bufPtr);
				bufPtr += 4;
				zipArchive->endOfCDR.diskNum = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				zipArchive->endOfCDR.startDiskNum = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				zipArchive->endOfCDR.totalEntriesOnDisk = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				zipArchive->endOfCDR.totalEntries = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				zipArchive->endOfCDR.size = (*(uint32_t*)bufPtr);
				bufPtr += 4;
				zipArchive->endOfCDR.startOffset = (*(uint32_t*)bufPtr);
				bufPtr += 4;
				zipArchive->endOfCDR.commentLength = (*(uint16_t*)bufPtr);
				bufPtr += 2;

				return true;
			}
		}
	}

	return false;
}

static CentralDirectory *loadCentralDirectory(ZipArchive *zipArchive) {
	FileBuffer *fileBuffer;
	FileHeader *fileHeader;
	EndOfCDR *endOfCDR;
	AIOFile *aioFile;
	void *bufPtr;

	endOfCDR = &zipArchive->endOfCDR;
	fileBuffer = ce97d170_containsData(&zipArchive->bufferList, endOfCDR->startOffset, endOfCDR->size);

	if (fileBuffer == NULL) {
		uint32_t dataLength;

		// Release all currently held FileBuffers
		ce97d170_resetFileBufferList(&zipArchive->bufferList, f502a409_releasePage);

		// Calculate the Direct I/O file offset and number of blocks to read
		aioFile = &zipArchive->aioFile;
		aioFile->offset = (endOfCDR->startOffset >> 9) << 9;
		dataLength = endOfCDR->startOffset - aioFile->offset + endOfCDR->size;

		// Read the data
		ce97d170_readFileBufferList(aioFile, &zipArchive->bufferList, dataLength);
		fileBuffer = ce97d170_containsData(&zipArchive->bufferList, endOfCDR->startOffset, dataLength);
	}

	bufPtr = fileBuffer->buffer + fileBuffer->dataOffset;

	for (uint32_t i=0; i < endOfCDR->totalEntries; i++) {
		if ( (*(uint32_t*)bufPtr) == ZIP_FILE_HEADER_SIG) {
			fileHeader = ce667b0d_createFileHeader();

			fileHeader->signature = (*(uint32_t*)bufPtr);
			bufPtr += 4;
			fileHeader->madeByVersion = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->needToExtractVersion = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->bitFlags = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->compressMethod = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->lastModFileTime = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->lastModFileDate = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->crc32 = (*(uint32_t*)bufPtr);
			bufPtr += 4;
			fileHeader->compressSize = (*(uint32_t*)bufPtr);
			bufPtr += 4;
			fileHeader->uncompressSize = (*(uint32_t*)bufPtr);
			bufPtr += 4;
			fileHeader->fileNameLen = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->extraFieldLen = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->fileCommentLen = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->diskNumStart = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->internalFileAttribs = (*(uint16_t*)bufPtr);
			bufPtr += 2;
			fileHeader->externalFileAttribs = (*(uint32_t*)bufPtr);
			bufPtr += 4;
			fileHeader->localHeaderOffset = (*(uint32_t*)bufPtr);
			bufPtr += 4;

			if (fileHeader->fileNameLen > 0) {
				if (zipArchive->outputDir == NULL) {
					fileHeader->fileName = f668c4bd_stralloc(fileHeader->fileNameLen);
					f668c4bd_memcopy(bufPtr, fileHeader->fileName, fileHeader->fileNameLen);
					fileHeader->fileName[fileHeader->fileNameLen] = '\0';
				} else {
					StringBuilder outputFileName;

					c598a24c_initStringBuilder(&outputFileName);

					c598a24c_append_string(&outputFileName, zipArchive->outputDir);

					if (c598a24c_getLastChar(&outputFileName) != '/') {
						c598a24c_append_char(&outputFileName, '/');
					}

					c598a24c_appendBytes(&outputFileName, bufPtr, fileHeader->fileNameLen);
					fileHeader->fileName = c598a24c_createString(&outputFileName, NULL);
					c598a24c_cleanUpStringBuilder(&outputFileName);
				}
				bufPtr += fileHeader->fileNameLen;
			}

			if (fileHeader->extraFieldLen > 0) {
				fileHeader->extraField = f668c4bd_stralloc(fileHeader->extraFieldLen);
				f668c4bd_memcopy(bufPtr, fileHeader->extraField, fileHeader->extraFieldLen);
				fileHeader->extraField[fileHeader->extraFieldLen] = '\0';
				bufPtr += fileHeader->extraFieldLen;
			}

			if (fileHeader->fileCommentLen > 0) {
				fileHeader->fileComment = f668c4bd_stralloc(fileHeader->fileCommentLen);
				f668c4bd_memcopy(bufPtr, fileHeader->fileComment, fileHeader->fileCommentLen);
				fileHeader->fileComment[fileHeader->fileCommentLen] = '\0';
				bufPtr += fileHeader->fileCommentLen;
			}

			b196167f_add(&zipArchive->centralDirectory.fileHeaderList, fileHeader);
		}
	}

	return &zipArchive->centralDirectory;
}

static void processFileHeaderList(ZipArchive *zipArchive, CentralDirectory *centralDir) {
	LocalFileHeader *localFileHeader;
	FileBuffer *fileBuffer;
	FileHeader *fileHeader;
	uint32_t dataLength;
	AIOFile *inputFile;
	time_t timestamp;
	uint32_t crc32;
	void *bufPtr;
	int fd;

	inputFile = &zipArchive->aioFile;

	for (uint32_t i=0; i < centralDir->fileHeaderList.length; i++) {
		fileHeader = b196167f_get(&centralDir->fileHeaderList, i);

		if (fileHeader->compressMethod == ZIP_METHOD_STORED && fileHeader->compressSize == 0) {

			// Create directory
			d0059b5b_makeDirectory(fileHeader->fileName, DIR_DEFAULT_MODE, false);

		} else {
			dataLength = ZIP_FILE_LOCAL_HEADER_SIZE + fileHeader->fileNameLen + fileHeader->extraFieldLen;
			fileBuffer = ce97d170_containsData(&zipArchive->bufferList, fileHeader->localHeaderOffset, dataLength);

			if (fileBuffer == NULL) {
				// Release all currently held FileBuffers
				ce97d170_resetFileBufferList(&zipArchive->bufferList, f502a409_releasePage);

				// Calculate the Direct I/O file offset and length of data to read
				inputFile->offset = (fileHeader->localHeaderOffset >> 9) << 9;
				dataLength += (fileHeader->localHeaderOffset - inputFile->offset);

				// Read the data
				ce97d170_readFileBufferList(inputFile, &zipArchive->bufferList, dataLength);
				fileBuffer = ce97d170_containsData(&zipArchive->bufferList, fileHeader->localHeaderOffset, dataLength);
			}

			bufPtr = fileBuffer->buffer + fileBuffer->dataOffset;

			if ( (*(uint32_t*)bufPtr) == ZIP_FILE_LOCAL_HEADER_SIG) {
				localFileHeader = ce667b0d_createLocalFileHeader();

				localFileHeader->signature = (*(uint32_t*)bufPtr);
				bufPtr += 4;
				localFileHeader->needToExtractVersion = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				localFileHeader->bitFlags = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				localFileHeader->compressMethod = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				localFileHeader->lastModFileTime = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				localFileHeader->lastModFileDate = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				localFileHeader->crc32 = (*(uint32_t*)bufPtr);
				bufPtr += 4;
				localFileHeader->compressSize = (*(uint32_t*)bufPtr);
				bufPtr += 4;
				localFileHeader->uncompressSize = (*(uint32_t*)bufPtr);
				bufPtr += 4;
				localFileHeader->fileNameLen = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				localFileHeader->extraFieldLen = (*(uint16_t*)bufPtr);
				bufPtr += 2;

				if (localFileHeader->fileNameLen > 0) {
					localFileHeader->fileName = f668c4bd_stralloc(localFileHeader->fileNameLen);
					f668c4bd_memcopy(bufPtr, localFileHeader->fileName, localFileHeader->fileNameLen);
					localFileHeader->fileName[localFileHeader->fileNameLen] = '\0';
					bufPtr += localFileHeader->fileNameLen;
				}

				if (localFileHeader->extraFieldLen > 0) {
					localFileHeader->extraField = f668c4bd_stralloc(localFileHeader->extraFieldLen);
					f668c4bd_memcopy(bufPtr, localFileHeader->extraField, localFileHeader->extraFieldLen);
					localFileHeader->extraField[localFileHeader->extraFieldLen] = '\0';
					bufPtr += localFileHeader->extraFieldLen;
				}
/*
				printf("LocalFileHeader #%u:\n", i);
				printf("\tfilename:             %s [%s]\n", localFileHeader->fileName, f6215943_isEqual(localFileHeader->fileName, fileHeader->fileName) ? "same" : "different");
				printf("\tsignature:            %#x\n", localFileHeader->signature);
				printf("\tneedToExtractVersion: %u [%s]\n", localFileHeader->needToExtractVersion, (localFileHeader->needToExtractVersion == fileHeader->needToExtractVersion) ? "same" : "different");
				printf("\tbitFlags:             %u [%s]\n", localFileHeader->bitFlags, (localFileHeader->bitFlags == fileHeader->bitFlags) ? "same" : "different");
				printf("\tcompressMethod:       %u [%s]\n", localFileHeader->compressMethod, (localFileHeader->compressMethod == fileHeader->compressMethod) ? "same" : "different");
				printf("\tlastModFileTime:      %u [%s]\n", localFileHeader->lastModFileTime, (localFileHeader->lastModFileTime == fileHeader->lastModFileTime) ? "same" : "different");
				printf("\tlastModFileDate:      %u [%s]\n", localFileHeader->lastModFileDate, (localFileHeader->lastModFileDate == fileHeader->lastModFileDate) ? "same" : "different");
				printf("\tcrc32:                %u [%s]\n", localFileHeader->crc32, (localFileHeader->crc32 == fileHeader->crc32) ? "same" : "different");
				printf("\tcompressSize:         %u [%s]\n", localFileHeader->compressSize, (localFileHeader->compressSize == fileHeader->compressSize) ? "same" : "different");
				printf("\tuncompressSize:       %u [%s]\n", localFileHeader->uncompressSize, (localFileHeader->uncompressSize == fileHeader->uncompressSize) ? "same" : "different");
				printf("\tfileNameLen:          %u [%s]\n", localFileHeader->fileNameLen, (localFileHeader->fileNameLen == fileHeader->fileNameLen) ? "same" : "different");
				printf("\textraFieldLen:        %u [%s]\n", localFileHeader->extraFieldLen, (localFileHeader->extraFieldLen == fileHeader->extraFieldLen) ? "same" : "different");
				printf("\n");
*/
				// Create any subdirectories to the file
				d0059b5b_makeDirectory(fileHeader->fileName, DIR_DEFAULT_MODE, true);

				if (fileHeader->compressMethod == ZIP_METHOD_STORED) {
					fileBuffer = ce97d170_containsData(&zipArchive->bufferList, fileHeader->localHeaderOffset + dataLength, fileHeader->compressSize);

					// Check CRC-32 of the FileBuffer
					crc32 = ce97d170_crc32(fileBuffer, fileHeader->uncompressSize);

					if (crc32 == fileHeader->crc32) {
						// Create the file
						fd = e2f74138_createFile(fileHeader->fileName, FOPEN_WRITEONLY, 0, FILE_DEFAULT_MODE);

						// Write out FileBuffer
						ce97d170_write(fileBuffer, fd, fileHeader->uncompressSize, fileHeader->fileName);

						// Modify the file timestamp
						timestamp = a66923ff_convertTimeFromDOS(fileHeader->lastModFileDate, fileHeader->lastModFileTime);
						e2f74138_setTimestamp(fd, fileHeader->fileName, timestamp, timestamp);

						// Close the file descriptor
						e2f74138_closeFile(fd, fileHeader->fileName);
					} else {

					}

				} else if (fileHeader->compressMethod == ZIP_METHOD_DEFLATE) {
					if (fileHeader->uncompressSize == 0) {
						// Create the empty file
						fd = e2f74138_createFile(fileHeader->fileName, FOPEN_WRITEONLY, 0, FILE_DEFAULT_MODE);

						// Modify the file timestamp
						timestamp = a66923ff_convertTimeFromDOS(fileHeader->lastModFileDate, fileHeader->lastModFileTime);
						e2f74138_setTimestamp(fd, fileHeader->fileName, timestamp, timestamp);

						// Close the file descriptor
						e2f74138_closeFile(fd, fileHeader->fileName);
					} else {
						// TODO: Inflate deflated file
					}
				}
			}
		}
	}
}

static void printEndOfCDR(EndOfCDR *endOfCDR) {
	printf("EndOfCDR Data:\n");
	printf("\tsignature:          %#x\n", endOfCDR->signature);
	printf("\tdiskNum:            %u\n", endOfCDR->diskNum);
	printf("\tstartDiskNum:       %u\n", endOfCDR->startDiskNum);
	printf("\ttotalEntriesOnDisk: %u\n", endOfCDR->totalEntriesOnDisk);
	printf("\ttotalEntries:       %u\n", endOfCDR->totalEntries);
	printf("\tsize:               %u\n", endOfCDR->size);
	printf("\tstartOffset:        %u\n", endOfCDR->startOffset);
	printf("\tcommentLength:      %u\n", endOfCDR->commentLength);
	printf("\n");
}

static void printCentralDirectory(CentralDirectory *centralDir) {
	FileHeader *fileHeader;
	uint32_t bitmask = 0x0001;

	for (uint32_t i=0; i < centralDir->fileHeaderList.length; i++) {
		fileHeader = b196167f_get(&centralDir->fileHeaderList, i);

		printf("FileHeader #%u:\n", i);
		printf("\tfilename:             %s\n", fileHeader->fileName);
		printf("\tsignature:            %#x\n", fileHeader->signature);
		printf("\tmadeByVersion:        %u\n", fileHeader->madeByVersion);
		printf("\tneedToExtractVersion: %u\n", fileHeader->needToExtractVersion);
		printf("\tbitFlags:             ");

		// Print out the meaning of the set bits
		if ((fileHeader->bitFlags & bitmask) == 1) {
			printf("encrypted ");
		}
		bitmask <<= 1;

		if (fileHeader->compressMethod == ZIP_METHOD_IMPLODE) {
			if ((fileHeader->bitFlags & bitmask) == 0) {
				printf("4K sliding ");
			} else {
				printf("8K sliding ");
			}
			bitmask <<= 1;

			if ((fileHeader->bitFlags & bitmask) == 0) {
				printf("2 SF Trees ");
			} else {
				printf("3 SF Trees ");
			}
			bitmask <<= 1;

		} else if (fileHeader->compressMethod == ZIP_METHOD_DEFLATE
		        || fileHeader->compressMethod == ZIP_METHOD_DEFLATE64) {

			uint32_t deflateOption = (fileHeader->bitFlags & ZIP_DEFLATE_MASK);
			bitmask <<= 2;

			if (deflateOption == ZIP_DEFLATE_SUPERFAST) {
				printf("Super Fast ");
			} else if (deflateOption == ZIP_DEFLATE_FAST) {
				printf("Fast ");
			} else if (deflateOption == ZIP_DEFLATE_MAXIMUM) {
				printf("Maximum ");
			} else if (deflateOption == ZIP_DEFLATE_NORMAL) {
				printf("Normal ");
			}
		}

		if ((fileHeader->bitFlags & bitmask) == 1) {
			printf("zero local header ");
		}
		printf("\n");

		printf("\tcompressMethod:       %u\n", fileHeader->compressMethod);
		printf("\tlastModFileTime:      %u\n", fileHeader->lastModFileTime);
		printf("\tlastModFileDate:      %u\n", fileHeader->lastModFileDate);
		printf("\tcrc32:                %u\n", fileHeader->crc32);
		printf("\tcompressSize:         %u\n", fileHeader->compressSize);
		printf("\tuncompressSize:       %u\n", fileHeader->uncompressSize);
		printf("\tfileNameLen:          %u\n", fileHeader->fileNameLen);
		printf("\textraFieldLen:        %u\n", fileHeader->extraFieldLen);
		printf("\tfileCommentLen:       %u\n", fileHeader->fileCommentLen);
		printf("\tdiskNumStart:         %u\n", fileHeader->diskNumStart);
		printf("\tinternalFileAttribs:  %u\n", fileHeader->internalFileAttribs);
		printf("\texternalFileAttribs:  %u\n", fileHeader->externalFileAttribs);
		printf("\tlocalHeaderOffset:    %u\n", fileHeader->localHeaderOffset);
		printf("\n");
	}
}

static FileHeader *ce667b0d_createFileHeader() {
	FileHeader *fileHeader = malloc(sizeof(FileHeader));

	f668c4bd_meminit(fileHeader, sizeof(FileHeader));

	return fileHeader;
}

static LocalFileHeader *ce667b0d_createLocalFileHeader() {
	LocalFileHeader *localFileHeader = malloc(sizeof(LocalFileHeader));

	f668c4bd_meminit(localFileHeader, sizeof(LocalFileHeader));

	return localFileHeader;
}
