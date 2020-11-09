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
#include "inflate.h"

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

#define ZIP64_END_OF_CDR_SIGNATURE  0x06064b50
#define ZIP64_END_OF_CDL_SIGNATURE  0x07064b50

// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef enum CompressionMethod {
	ZIP_METHOD_STORED = 0,
	ZIP_METHOD_IMPLODE = 6,
	ZIP_METHOD_DEFLATE = 8,
	ZIP_METHOD_DEFLATE64 = 9
} CompressionMethod;

/*
 * Bit 2  Bit 1
 *   0      0    Normal (-en) compression option was used
 *   0      1    Maximum (-exx/-ex) compression option was used
 *   1      0    Fast (-ef) compression option was used
 *   1      1    Super Fast (-es) compression option was used
 */
typedef enum DeflateOption {
	ZIP_DEFLATE_MASK = 6,
	ZIP_DEFLATE_NORMAL = 0,
	ZIP_DEFLATE_MAXIMUM = 2,
	ZIP_DEFLATE_FAST = 4,
	ZIP_DEFLATE_SUPERFAST = 6
} DeflateOption;

/*
 * Local File Header
 *   - local file header signature                        4 bytes  (0x04034b50)
 *   - version needed to extract                          2 bytes
 *   - general purpose bit flag                           2 bytes
 *   - compression method                                 2 bytes
 *   - last mod file time                                 2 bytes
 *   - last mod file date                                 2 bytes
 *   - crc-32                                             4 bytes
 *   - compressed size                                    4 bytes
 *   - uncompressed size                                  4 bytes
 *   - file name length                                   2 bytes
 *   - extra field length                                 2 bytes
 *
 *   - file name                                          (variable size)
 *   - extra field                                        (variable size)
 */
typedef struct LocalFileHeader {
	char       *fileName;
	char       *extraField;
	char       *fileData;
	uint32_t    signature;
	uint32_t    crc32;
	uint32_t    compressSize;
	uint32_t    uncompressSize;
	uint16_t    needToExtractVersion;
	uint16_t    bitFlags;
	uint16_t    compressMethod;
	uint16_t    lastModFileTime;
	uint16_t    lastModFileDate;
	uint16_t    fileNameLen;
	uint16_t    extraFieldLen;
} LocalFileHeader __attribute__ ((aligned (16)));

#if __SIZEOF_POINTER__ == 8
static_assert(sizeof(LocalFileHeader) == 56, "Check your assumptions");
#elif  __SIZEOF_POINTER__ == 4
static_assert(sizeof(LocalFileHeader) == 44, "Check your assumptions");
#endif

/*
 * Data Descriptor
 *   - crc-32                                             4 bytes
 *   - compressed size                                    4 bytes
 *   - uncompressed size                                  4 bytes
 */
typedef struct DataDescriptor {
	uint32_t crc32;
	uint32_t compressSize;
	uint32_t uncompressSize;
} DataDescriptor;

static_assert(sizeof(DataDescriptor) == 12, "Check your assumptions");

/*
 * Zip64 Data Descriptor
 *   - crc-32                                             4 bytes
 *   - compressed size                                    8 bytes
 *   - uncompressed size                                  8 bytes
 */
typedef struct Zip64DataDescriptor {
	uint64_t compressSize;
	uint64_t uncompressSize;
	uint32_t crc32;
} Zip64DataDescriptor;

static_assert(sizeof(Zip64DataDescriptor) == 24, "Check your assumptions");

/*
 * File Header
 *   - central file header signature                      4 bytes  (0x02014b50)
 *   - version made by                                    2 bytes
 *   - version needed to extract                          2 bytes
 *   - general purpose bit flag                           2 bytes
 *   - compression method                                 2 bytes
 *   - last mod file time                                 2 bytes
 *   - last mod file date                                 2 bytes
 *   - crc-32                                             4 bytes
 *   - compressed size                                    4 bytes
 *   - uncompressed size                                  4 bytes
 *   - file name length                                   2 bytes
 *   - extra field length                                 2 bytes
 *   - file comment length                                2 bytes
 *   - disk number start                                  2 bytes
 *   - internal file attributes                           2 bytes
 *   - external file attributes                           4 bytes
 *   - relative offset of local header                    4 bytes
 *
 *   - file name                                          (variable size)
 *   - extra field                                        (variable size)
 *   - file comment                                       (variable size)
 */
typedef struct FileHeader {
	char*    fileName;
	char*    extraField;
	char*    fileComment;
	uint32_t signature;
	uint32_t crc32;
	uint32_t compressSize;
	uint32_t uncompressSize;
	uint32_t externalFileAttribs;
	uint32_t localHeaderOffset;
	uint16_t madeByVersion;
	uint16_t needToExtractVersion;
	uint16_t bitFlags;
	uint16_t compressMethod;
	uint16_t lastModFileTime;
	uint16_t lastModFileDate;
	uint16_t fileNameLen;
	uint16_t extraFieldLen;
	uint16_t fileCommentLen;
	uint16_t diskNumStart;
	uint16_t internalFileAttribs;
} FileHeader __attribute__ ((aligned (16)));

#if __SIZEOF_POINTER__ == 8
static_assert(sizeof(FileHeader) == 72, "Check your assumptions");
#elif  __SIZEOF_POINTER__ == 4
static_assert(sizeof(FileHeader) == 60, "Check your assumptions");
#endif

/*
 * Digital Signature
 *   - header signature                                   4 bytes  (0x05054b50)
 *   - size of data                                       2 bytes
 *   - signature data                                     (variable size)
 */
typedef struct DigitalSignature {
	char*    data;
	uint32_t signature;
	uint16_t dataSize;
} DigitalSignature;

#if __SIZEOF_POINTER__ == 8
static_assert(sizeof(DigitalSignature) == 16, "Check your assumptions");
#elif  __SIZEOF_POINTER__ == 4
static_assert(sizeof(DigitalSignature) == 12, "Check your assumptions");
#endif

/*
 * Central Directory Structure
 *   [file header 1]
 *   .
 *   .
 *   .
 *   [file header n]
 *   [digital signature]
 */
typedef struct CentralDirectory {
	ListArray        fileHeaderList;
	DigitalSignature digitalSignature;
} CentralDirectory;

#if __SIZEOF_POINTER__ == 8
static_assert(sizeof(CentralDirectory) == 32, "Check your assumptions");
#elif  __SIZEOF_POINTER__ == 4
static_assert(sizeof(CentralDirectory) == 24, "Check your assumptions");
#endif

/*
 * Zip64 End of Central Directory Record
 *   - zip64 end of central dir signature                 4 bytes  (0x06064b50)
 *   - size of zip64 end of central directory record      8 bytes
 *   - version made by                                    2 bytes
 *   - version needed to extract                          2 bytes
 *   - number of this disk                                4 bytes
 *   - number of the disk with the start of the
 *     central directory                                  4 bytes
 *   - total number of entries in the central
 *     directory on this disk                             8 bytes
 *   - total number of entries in the central
 *     directory                                          8 bytes
 *   - size of the central directory                      8 bytes
 *   - offset of start of central directory
 *     with respect to the starting disk number           8 bytes
 *   - zip64 extensible data sector                       (variable size)
 */
typedef struct Zip64EndOfCDR {
	uint64_t recordSize;
	uint64_t totalCentralDirEntriesOnThisDisk;
	uint64_t totalCentralDirEntries;
	uint64_t centralDirSize;
	uint64_t centralDirStartOffset;
	char*    extensibleData;
	uint32_t signature;
	uint32_t diskNum;
	uint32_t centralDirStartDiskNum;
	uint16_t madeByVersion;
	uint16_t needToExtractVersion;
} Zip64EndOfCDR;

static_assert(sizeof(Zip64EndOfCDR) == 64, "Check your assumptions");

/*
 * Zip64 End of Central Directory Locator
 *   - zip64 end of central dir locator signature         4 bytes  (0x07064b50)
 *   - number of the disk with the start of the zip64
 *     end of central directory                           4 bytes
 *   - relative offset of the zip64 end of central
 *     directory record                                   8 bytes
 *   - total number of disks                              4 bytes
 */
typedef struct Zip64EndOfCDL {
	uint64_t zip64EndOfCDRStartOffset;
	uint32_t zip64EndOfCDRStartDiskNum;
	uint32_t totalNumDisks;
} Zip64EndOfCDL;

static_assert(sizeof(Zip64EndOfCDL) == 16, "Check your assumptions");

/*
 * End of Central Directory Record
 *   - end of central dir signature                       4 bytes  (0x06054b50)
 *   - number of this disk                                2 bytes
 *   - number of the disk with the start of the
 *     central directory                                  2 bytes
 *   - total number of entries in the central
 *     directory on this disk                             2 bytes
 *   - total number of entries in the central
 *     directory                                          2 bytes
 *   - size of the central directory                      4 bytes
 *   - offset of start of central directory
 *     with respect to the starting disk number           4 bytes
 *   - .ZIP file comment length                           2 bytes
 *   - .ZIP file comment                                  (variable size)
 */
typedef struct EndOfCDR {
	char*    comment;
	uint32_t signature;
	uint32_t size;
	uint32_t startOffset;
	uint16_t diskNum;
	uint16_t startDiskNum;
	uint16_t totalEntriesOnDisk;
	uint16_t totalEntries;
	uint16_t commentLength;
} EndOfCDR;

#if __SIZEOF_POINTER__ == 8
static_assert(sizeof(EndOfCDR) == 32, "Check your assumptions");
#elif  __SIZEOF_POINTER__ == 4
static_assert(sizeof(EndOfCDR) == 28, "Check your assumptions");
#endif

/*
 * Zip Archive Format
 *   [local file header 1]
 *   [file data 1]
 *   [data descriptor 1]
 *   .
 *   .
 *   .
 *   [local file header n]
 *   [file data n]
 *   [data descriptor n]
 *   [central directory]
 *   [zip64 end of central directory record]
 *   [zip64 end of central directory locator]
 *   [end of central directory record]
 */
typedef struct ZipFormat {
	ZipArchive      *zipArchive;
	CentralDirectory centralDirectory;
	Zip64EndOfCDR    zip64EndOfCDR;
	Zip64EndOfCDL    zip64EndOfCDL;
	EndOfCDR         endOfCDR;
} ZipFormat;

// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static bool findEndOfCDR(ZipFormat *zipFormat);
static void loadCentralDirectory(ZipFormat *zipFormat);
static void processFileHeaderList(ZipArchive *zipArchive, CentralDirectory *centralDir);

static void printEndOfCDR(EndOfCDR *endOfCDR);
static void printCentralDirectory(CentralDirectory *centralDir);
static void printLocalFileHeader(LocalFileHeader *localFileHeader, FileHeader *fileHeader, uint32_t index);

static FileHeader *ce667b0d_createFileHeader();
static void ce667b0d_destroyFileHeader(void *fileHeader);
static LocalFileHeader *ce667b0d_createLocalFileHeader();
static void ce667b0d_destroyLocalFileHeader(LocalFileHeader *localFileHeader);

extern void ce667b0d_mapEndOfCDR(EndOfCDR *endOfCDR, void *bufferPtr);

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

	// 2. Clean up the FileBufferList struct
	ce97d170_cleanUpFileBufferList(&zipArchive->bufferList, f502a409_releasePage);
}

void ce667b0d_initZipArchive(ZipArchive *zipArchive, AIOContext *aioContext, char *fileName) {
	FileStatus fileStatus;

	// 1. Initialize the FileBufferList struct
	ce97d170_initFileBufferList(&zipArchive->bufferList);

	// 2. Initialize the AIOFile struct
	f1207515_initAIOFile(aioContext, &zipArchive->aioFile, fileName);

	// 3. Initialize the AIOContext and output directory
	zipArchive->aioContext = aioContext;
	zipArchive->outputDir = NULL;

	// 4. Open the file
	f1207515_open(&zipArchive->aioFile, FOPEN_READONLY, 0);

	// 5. Retrieve the file size
	e2f74138_getDescriptorStatus(zipArchive->aioFile.fd, &fileStatus);
	zipArchive->aioFile.fileSize = fileStatus.st_size;
}

static void cleanUpZipFormat(ZipFormat *zipFormat) {
	// 1. Clean up the Central Directory FileHeader list
	b196167f_cleanUpListArray(&zipFormat->centralDirectory.fileHeaderList, ce667b0d_destroyFileHeader);

	// 2. Clean up the Central Directory DigitalSignature data
	if (zipFormat->centralDirectory.digitalSignature.data != NULL) {
		f668c4bd_free(zipFormat->centralDirectory.digitalSignature.data);
	}

	// 3. Clean up the End of Central Directory struct
	if (zipFormat->endOfCDR.comment != NULL) {
		f668c4bd_free(zipFormat->endOfCDR.comment);
	}
}

static void initZipFormat(ZipFormat *zipFormat, ZipArchive *zipArchive) {
	// 1. Initialize the ZipFormat memory
	f668c4bd_meminit(zipFormat, sizeof(ZipFormat));

	// 2. Initialize the Central Directory FileHeader list
	b196167f_initListArray(&zipFormat->centralDirectory.fileHeaderList);

	// 3. Initialize the ZipArchive struct
	zipFormat->zipArchive = zipArchive;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ce667b0d_unzip(ZipArchive *zipArchive) {
	ZipFormat zipFormat;

	// 1. Initialize the ZipFormat struct
	initZipFormat(&zipFormat, zipArchive);

	// 2. Find the End of Central Directory Record
	if (findEndOfCDR(&zipFormat)) {
		printEndOfCDR(&zipFormat.endOfCDR);

		// 3. Load the Central Directory
		loadCentralDirectory(&zipFormat);

		// printCentralDirectory(&zipFormat.centralDirectory);

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
		processFileHeaderList(zipArchive, &zipFormat.centralDirectory);
	}

	// 6. Clean up ZipFormat struct
	cleanUpZipFormat(&zipFormat);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static bool findEndOfCDR(ZipFormat *zipFormat) {
	FileBuffer *fileBuffer;
	AIOFile *aioFile;
	uint32_t length;
	void *bufPtr;

	// 1. Set the initial offset
	aioFile = &zipFormat->zipArchive->aioFile;
	if (aioFile->fileSize > MEMORY_PAGE_SIZE) {
		aioFile->offset = aioFile->fileSize - MEMORY_PAGE_SIZE;
		aioFile->offset = ((aioFile->offset + 511) >> 9) << 9;
	}

	length = aioFile->fileSize - aioFile->offset;

	// 2. Read end of Zip archive
	fileBuffer = ce97d170_readFileBuffer(aioFile, length);
	ce97d170_addBuffer(&zipFormat->zipArchive->bufferList, fileBuffer);

	bufPtr = fileBuffer->buffer + fileBuffer->numBytes;

	if (fileBuffer->numBytes > ZIP_END_OF_CDR_SIZE) {
		for (bufPtr -= ZIP_END_OF_CDR_SIZE; bufPtr >= fileBuffer->buffer; bufPtr--) {
			if ( (*(uint32_t*)bufPtr) == ZIP_END_OF_CDR_SIG) {
				ce667b0d_mapEndOfCDR(&zipFormat->endOfCDR, bufPtr);
/*
				zipFormat->endOfCDR.signature = (*(uint32_t*)bufPtr);
				bufPtr += 4;
				zipFormat->endOfCDR.diskNum = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				zipFormat->endOfCDR.startDiskNum = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				zipFormat->endOfCDR.totalEntriesOnDisk = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				zipFormat->endOfCDR.totalEntries = (*(uint16_t*)bufPtr);
				bufPtr += 2;
				zipFormat->endOfCDR.size = (*(uint32_t*)bufPtr);
				bufPtr += 4;
				zipFormat->endOfCDR.startOffset = (*(uint32_t*)bufPtr);
				bufPtr += 4;
				zipFormat->endOfCDR.commentLength = (*(uint16_t*)bufPtr);
				bufPtr += 2;
*/
				return true;
			}
		}
	}

	return false;
}

static void loadCentralDirectory(ZipFormat *zipFormat) {
	FileBuffer *fileBuffer;
	FileHeader *fileHeader;
	ZipArchive *zipArchive;
	EndOfCDR *endOfCDR;
	AIOFile *aioFile;
	void *bufPtr;

	endOfCDR = &zipFormat->endOfCDR;
	zipArchive = zipFormat->zipArchive;
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

			b196167f_add(&zipFormat->centralDirectory.fileHeaderList, fileHeader);
		}
	}
}

static void processFileHeaderList(ZipArchive *zipArchive, CentralDirectory *centralDir) {
	LocalFileHeader *localFileHeader;
	FileBuffer *fileBuffer;
	FileHeader *fileHeader;
	Inflate inflateData;
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
						// TODO: CRC-32 does not match the one in the archive
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
						// printLocalFileHeader(localFileHeader, fileHeader, i);
						fileBuffer = ce97d170_containsData(&zipArchive->bufferList, fileHeader->localHeaderOffset + dataLength, fileHeader->compressSize);

						d592eb82_initInflate(&inflateData, fileBuffer, fileHeader->compressSize);
						d592eb82_inflate(&inflateData);

						// Check CRC-32 of the OutputBuffer
						crc32 = c49f5b0d_crc32(&inflateData.outputBuffer, fileHeader->uncompressSize);

						if (crc32 == fileHeader->crc32) {
							// Create the file
							fd = e2f74138_createFile(fileHeader->fileName, FOPEN_WRITEONLY, 0, FILE_DEFAULT_MODE);

							// Write out OutputBuffer
							c49f5b0d_write(&inflateData.outputBuffer, fd, fileHeader->fileName);

							// Modify the file timestamp
							timestamp = a66923ff_convertTimeFromDOS(fileHeader->lastModFileDate, fileHeader->lastModFileTime);
							e2f74138_setTimestamp(fd, fileHeader->fileName, timestamp, timestamp);

							// Close the file descriptor
							e2f74138_closeFile(fd, fileHeader->fileName);
						} else {
							printf("CRC-32 does not match!");
						}

						d592eb82_cleanUpInflate(&inflateData);
					}
				}

				ce667b0d_destroyLocalFileHeader(localFileHeader);
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

static void printLocalFileHeader(LocalFileHeader *localFileHeader, FileHeader *fileHeader, uint32_t index) {
	printf("LocalFileHeader #%u:\n", index);
	printf("\tfilename:             %s\n", localFileHeader->fileName);
	printf("\tsignature:            %#x\n", localFileHeader->signature);
	printf("\tneedToExtractVersion: %u\n", localFileHeader->needToExtractVersion);
	printf("\tbitFlags:             %u\n", localFileHeader->bitFlags);
	printf("\tcompressMethod:       %u\n", localFileHeader->compressMethod);
	printf("\tlastModFileTime:      %u\n", localFileHeader->lastModFileTime);
	printf("\tlastModFileDate:      %u\n", localFileHeader->lastModFileDate);
	printf("\tcrc32:                %u\n", fileHeader->crc32);
	printf("\tcompressSize:         %u\n", fileHeader->compressSize);
	printf("\tuncompressSize:       %u\n", fileHeader->uncompressSize);
	printf("\tfileNameLen:          %u\n", localFileHeader->fileNameLen);
	printf("\textraFieldLen:        %u\n", localFileHeader->extraFieldLen);
	printf("\n");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

static FileHeader *ce667b0d_createFileHeader() {
	FileHeader *fileHeader = f668c4bd_malloc(sizeof(FileHeader));

	f668c4bd_meminit(fileHeader, sizeof(FileHeader));

	return fileHeader;
}

static void ce667b0d_destroyFileHeader(void *fileHeaderPtr) {
	FileHeader *fileHeader = (FileHeader *)fileHeaderPtr;

	if (fileHeader->fileName != NULL) {
		f668c4bd_free(fileHeader->fileName);
	}

	if (fileHeader->extraField != NULL) {
		f668c4bd_free(fileHeader->extraField);
	}

	if (fileHeader->fileComment != NULL) {
		f668c4bd_free(fileHeader->fileComment);
	}

	f668c4bd_free(fileHeader);
}

static LocalFileHeader *ce667b0d_createLocalFileHeader() {
	LocalFileHeader *localFileHeader = f668c4bd_malloc(sizeof(LocalFileHeader));

	f668c4bd_meminit(localFileHeader, sizeof(LocalFileHeader));

	return localFileHeader;
}

static void ce667b0d_destroyLocalFileHeader(LocalFileHeader *localFileHeader) {
	if (localFileHeader->fileName != NULL) {
		f668c4bd_free(localFileHeader->fileName);
	}

	if (localFileHeader->extraField != NULL) {
		f668c4bd_free(localFileHeader->extraField);
	}

	f668c4bd_free(localFileHeader);
}
