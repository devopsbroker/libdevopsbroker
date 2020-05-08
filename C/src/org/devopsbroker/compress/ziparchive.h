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

#define FILE_HEADER_SIGNATURE       0x02014b50
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

static_assert(sizeof(LocalFileHeader) == 56, "Check your assumptions");

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

static_assert(sizeof(FileHeader) == 72, "Check your assumptions");

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

static_assert(sizeof(DigitalSignature) == 16, "Check your assumptions");

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

static_assert(sizeof(CentralDirectory) == 32, "Check your assumptions");

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

static_assert(sizeof(EndOfCDR) == 32, "Check your assumptions");

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
typedef struct ZipArchive {
	FileBufferList   bufferList;
	AIOFile          aioFile;
	char            *outputDir;


	CentralDirectory centralDirectory;
	Zip64EndOfCDR    zip64EndOfCDR;
	Zip64EndOfCDL    zip64EndOfCDL;
	EndOfCDR         endOfCDR;
} ZipArchive;

//static_assert(sizeof(ZipArchive) == 64, "Check your assumptions");

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
