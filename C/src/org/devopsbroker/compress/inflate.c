/*
 * inflate.c - DevOpsBroker C source file for the org.devopsbroker.compress.Inflate struct
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
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-51
 *
 * The inflate code comes from hwzip-1.2.zip which can be found at the great
 * blog post on the DEFLATE algorithm at https://www.hanshq.net/zip.html
 *
 * I have made some customizations to the code to get it to work with the
 * libdevopsbroker environment, as well as some custom optimizations. I will
 * continue to make adjustments to the code as time permits.
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _GNU_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include <assert.h>

#include "inflate.h"
#include "bits.h"
#include "huffman.h"

#include "../lang/byte.h"
#include "../lang/error.h"
#include "../lang/memory.h"
#include "../lang/short.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define ISTREAM_MIN_BITS (64 - 7)

#define LITLEN_EOB 256
#define LITLEN_MAX 285
#define LITLEN_TBL_OFFSET 257
#define MIN_LEN 3
#define MAX_LEN 258

#define DISTSYM_MAX 29
#define MIN_DISTANCE 1
#define MAX_DISTANCE 32768

#define MIN_CODELEN_LENS 4
#define MAX_CODELEN_LENS 19

#define MIN_LITLEN_LENS 257
#define MAX_LITLEN_LENS 288

#define MIN_DIST_LENS 1
#define MAX_DIST_LENS 32

#define CODELEN_MAX_LIT 15

#define CODELEN_COPY 16
#define CODELEN_COPY_MIN 3
#define CODELEN_COPY_MAX 6

#define CODELEN_ZEROS 17
#define CODELEN_ZEROS_MIN 3
#define CODELEN_ZEROS_MAX 10

#define CODELEN_ZEROS2 18
#define CODELEN_ZEROS2_MIN 11
#define CODELEN_ZEROS2_MAX 138

// ═════════════════════════════════ Typedefs ═════════════════════════════════

/*
 * Table of litlen symbol values minus 257 with corresponding base length and
 * number of extra bits.
 */
typedef struct LitlenSymbol {
	uint16_t baseLength : 9;
	uint16_t extraBits : 7;
} LitlenSymbol;

static_assert(sizeof(LitlenSymbol) == 2, "Check your assumptions");

/*
 * Table of dist symbol values with corresponding base distance and number of
 * extra bits.
 */
typedef struct DistSymbol {
	uint32_t baseDist : 16;
	uint32_t extraBits : 16;
} DistSymbol;

static_assert(sizeof(DistSymbol) == 4, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════

static const int codelenLengthsOrder[MAX_CODELEN_LENS] =
{ 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

const struct LitlenSymbol litlen_tbl[29] = {
	{ 3, 0 },                     // 257
	{ 4, 0 },                     // 258
	{ 5, 0 },                     // 259
	{ 6, 0 },                     // 260
	{ 7, 0 },                     // 261
	{ 8, 0 },                     // 262
	{ 9, 0 },                     // 263
	{ 10, 0 },                    // 264
	{ 11, 1 },                    // 265
	{ 13, 1 },                    // 266
	{ 15, 1 },                    // 267
	{ 17, 1 },                    // 268
	{ 19, 2 },                    // 269
	{ 23, 2 },                    // 270
	{ 27, 2 },                    // 271
	{ 31, 2 },                    // 272
	{ 35, 3 },                    // 273
	{ 43, 3 },                    // 274
	{ 51, 3 },                    // 275
	{ 59, 3 },                    // 276
	{ 67, 4 },                    // 277
	{ 83, 4 },                    // 278
	{ 99, 4 },                    // 279
	{ 115, 4 },                   // 280
	{ 131, 5 },                   // 281
	{ 163, 5 },                   // 282
	{ 195, 5 },                   // 283
	{ 227, 5 },                   // 284
	{ 258, 0 }                    // 285
};

const struct DistSymbol dist_tbl[30] = {
	{ 1, 0 },                      // 0
	{ 2, 0 },                      // 1
	{ 3, 0 },                      // 2
	{ 4, 0 },                      // 3
	{ 5, 1 },                      // 4
	{ 7, 1 },                      // 5
	{ 9, 2 },                      // 6
	{ 13, 2 },                     // 7
	{ 17, 3 },                     // 8
	{ 25, 3 },                     // 9
	{ 33, 4 },                     // 10
	{ 49, 4 },                     // 11
	{ 65, 5 },                     // 12
	{ 97, 5 },                     // 13
	{ 129, 6 },                    // 14
	{ 193, 6 },                    // 15
	{ 257, 7 },                    // 16
	{ 385, 7 },                    // 17
	{ 513, 8 },                    // 18
	{ 769, 8 },                    // 19
	{ 1025, 9 },                   // 20
	{ 1537, 9 },                   // 21
	{ 2049, 10 },                  // 22
	{ 3073, 10 },                  // 23
	{ 4097, 11 },                  // 24
	{ 6145, 11 },                  // 25
	{ 8193, 12 },                  // 26
	{ 12289, 12 },                 // 27
	{ 16385, 13 },                 // 28
	{ 24577, 13 }                  // 29
};


// ════════════════════════════ Function Prototypes ═══════════════════════════

static bool processNonCompressionBlock(Inflate *inflate);
static bool processFixedDeflateBlock(Inflate *inflate);
static bool processDynamicDeflateBlock(Inflate *inflate);
static bool processLiteralBackrefBlock(Inflate *inflate, HuffmanDecoder *litlenDecoder, HuffmanDecoder *distDecoder);

static bool initDynamicDecoder(Inflate *inflate, HuffmanDecoder *litlenDecoder, HuffmanDecoder *distDecoder);





// Move to somewhere else, or code in assembly

/* Round x up to the next multiple of m, which must be a power of 2. */
static inline size_t round_up(size_t x, size_t m) {
	return (x + m - 1) & (size_t)(-m); /* Hacker's Delight (2nd), 3-1. */
}





// Move to lz77.h
static void output_backref64(OutputBuffer *outputBuffer, size_t dist, size_t length);
static void lz77_output_backref(OutputBuffer *outputBuffer, size_t dist, size_t length);

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

Inflate *d592eb82_createInflate(FileBuffer *fileBuffer, uint32_t compressSize) {
	Inflate *inflate = f668c4bd_malloc(sizeof(Inflate));

	// Initialize InputBuffer
	c49f5b0d_initInputBuffer(&inflate->inputBuffer, fileBuffer, compressSize);

	// Initialize OutputBuffer
	c49f5b0d_initOutputBuffer(&inflate->outputBuffer);

	// Initialize status
	inflate->inputBlockStart = 0;
	inflate->status = INFLATE_SUCCESS;

	return inflate;
}

void d592eb82_destroyInflate(Inflate *inflate) {
	// Clean up OutputBuffer
	c49f5b0d_cleanUpOutputBuffer(&inflate->outputBuffer);

	// Free Inflate
	f668c4bd_free(inflate);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void d592eb82_cleanUpInflate(Inflate *inflate) {
	// Clean up OutputBuffer struct
	c49f5b0d_cleanUpOutputBuffer(&inflate->outputBuffer);
}

void d592eb82_initInflate(Inflate *inflate, FileBuffer *fileBuffer, uint32_t compressSize) {
	// Initialize InputBuffer
	c49f5b0d_initInputBuffer(&inflate->inputBuffer, fileBuffer, compressSize);

	// Initialize OutputBuffer
	c49f5b0d_initOutputBuffer(&inflate->outputBuffer);

	// Initialize status
	inflate->inputBlockStart = 0;
	inflate->status = INFLATE_SUCCESS;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool d592eb82_inflate(Inflate *inflate) {
	uint32_t blockType;
	bool isFinalBlock;
	bool okStatus;

	okStatus = true;

	do {
		// Read the 3-bit block header value
		if (!c49f5b0d_useNumBits(&inflate->inputBuffer, 3)) {
			inflate->status = INFLATE_INPUT_ERROR;
			return false;
		}
		isFinalBlock = inflate->inputBuffer.bits & 0x01;
		blockType = inflate->inputBuffer.bits & 0x06;
		inflate->inputBuffer.bits >>= 3;

		if (blockType == 0) {
			// No compression
			okStatus = processNonCompressionBlock(inflate);
		} else if (blockType == 2) {
			// Compression with fixed Huffman codes
			okStatus = processFixedDeflateBlock(inflate);
		} else if (blockType == 4) {
			// Compression with dynamic Huffman codes
			okStatus = processDynamicDeflateBlock(inflate);
		} else {
			// Invalid block type
			inflate->status = INFLATE_INPUT_ERROR;
			return false;
		}
	} while (!isFinalBlock && okStatus);

	if (okStatus) {
		inflate->status = INFLATE_SUCCESS;
	}

	return okStatus;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static bool processNonCompressionBlock(Inflate *inflate) {
//	uint16_t blockLength, checksum;

	puts("Non-compression deflate block!");
/*
	// 1. Move input position to the next byte
	inflate->inputStream.buffer++;
	//inflate->inputBufPos++;

	// 2. Read block length and checksum
	if ((inflate->inputBufPos + 4) >= inflate->inputBufLen) {
		inflate->status = INFLATE_NEED_INPUT;
		return false;
	}

	blockLength = (*(uint16_t*)inflate->inputStream.buffer);
	inflate->inputStream.buffer += 2;
	checksum = (*(uint16_t*)inflate->inputStream.buffer);
	inflate->inputStream.buffer += 2;
	//inflate->inputBufPos += 4;

	printf("Block Length: %u\nChecksum: %u\n", blockLength, checksum);

	// 3. Compare the length to the checksum
	if (blockLength != ~checksum) {
		inflate->status = INFLATE_INPUT_ERROR;
		return false;
	}

	// 4. Make sure we have enough input to read uncompressed data block
	if ((inflate->inputBufPos + blockLength) >= inflate->inputBufLen) {
		inflate->status = INFLATE_NEED_INPUT;
		return false;
	}

	// 5. Make sure there is enough room in the output for the uncompressed data block
	if ((inflate->outputBufPos + blockLength) >= INFLATE_OUTPUT_LENGTH) {
		inflate->status = INFLATE_OUTPUT_FULL;
		return false;
	}

	f668c4bd_memcopy(inflate->inputStream.buffer, inflate->outputPtr, blockLength);
	//inflate->inputBufPos += blockLength;
	inflate->outputBufPos += blockLength;
*/
	return true;
}

static bool processFixedDeflateBlock(Inflate *inflate) {
	puts("Fixed deflate block!");

	return true;
}

static bool processDynamicDeflateBlock(Inflate *inflate) {
	HuffmanDecoder litlenDecoder, distDecoder;
	bool isSuccess = false;

	// printf("Dynamic deflate block!\n");

	// 1. Initialize the dynamic litlen and dist Huffman decoders
	isSuccess = initDynamicDecoder(inflate, &litlenDecoder, &distDecoder);

	if (!isSuccess) {
		return false;
	}

	// 2. Process the literals and backrefs block
	isSuccess = processLiteralBackrefBlock(inflate, &litlenDecoder, &distDecoder);

	return isSuccess;
}

static bool initDynamicDecoder(Inflate *inflate, HuffmanDecoder *litlenDecoder, HuffmanDecoder *distDecoder) {
	HuffmanDecoder codelenDecoder;
	uint8_t codelenLengthArray[MAX_CODELEN_LENS];
	uint8_t codeLengthArray[MAX_LITLEN_LENS + MAX_DIST_LENS];
	uint32_t numCodelenLens, numLitlenLens, numDistLens;
	size_t numBitsUsed;
	uint32_t i, j;
	int symbol;

	// Read the 14-bit dynamic Huffman codes
	if (!c49f5b0d_useNumBits(&inflate->inputBuffer, 14)) {
		inflate->status = INFLATE_NEED_INPUT;
		return false;
	}

	// Number of litlen codeword lengths (5 bits + 257)
	numLitlenLens = ((inflate->inputBuffer.bits & 0x1F) + MIN_LITLEN_LENS);
	inflate->inputBuffer.bits >>= 5;
	if (numLitlenLens > MAX_LITLEN_LENS) {
		inflate->status = INFLATE_INPUT_ERROR;
		return false;
	}

	// Number of dist codeword lengths (5 bits + 1)
	numDistLens = ((inflate->inputBuffer.bits & 0x1F) + MIN_DIST_LENS);
	inflate->inputBuffer.bits >>= 5;
	if (numDistLens > MAX_DIST_LENS) {
		inflate->status = INFLATE_INPUT_ERROR;
		return false;
	}

	// Number of code length lengths (4 bits + 4)
	numCodelenLens = ((inflate->inputBuffer.bits & 0x0F) + MIN_CODELEN_LENS);
	inflate->inputBuffer.bits >>= 4;
	if (numCodelenLens > MAX_CODELEN_LENS) {
		inflate->status = INFLATE_INPUT_ERROR;
		return false;
	}

//	printf("Num Litlen Lengths: %u\n", numLitlenLens);
//	printf("Num Dist Lengths: %u\n", numDistLens);
//	printf("Num Codelen Lengths: %u\n", numCodelenLens);

	// Initialize the codelen decoder by reading the 3-bit codelen codeword lengths
	for (i=0; i < numCodelenLens; i++) {
		if (!c49f5b0d_useNumBits(&inflate->inputBuffer, 3)) {
			inflate->status = INFLATE_NEED_INPUT;
			return false;
		}

		codelenLengthArray[codelenLengthsOrder[i]] = (uint8_t) (inflate->inputBuffer.bits & 0x07);
		inflate->inputBuffer.bits >>= 3;
	}

	for (; i < MAX_CODELEN_LENS; i++) {
		codelenLengthArray[codelenLengthsOrder[i]] = 0;
	}

//	d9352b73_printByteArray(codelenLengthArray, MAX_CODELEN_LENS);
//	printf("\n");

	f173ab5a_initHuffmanDecoder(&codelenDecoder, codelenLengthArray, MAX_CODELEN_LENS);

	// Read the litlen and dist codeword lengths
	i = 0;
	while (i < numLitlenLens + numDistLens) {
		c49f5b0d_getNextBits(&inflate->inputBuffer);
		symbol = f173ab5a_huffmanDecode(&inflate->inputBuffer, &codelenDecoder, &numBitsUsed);
		inflate->inputBuffer.bits >>= numBitsUsed;

		if (!c49f5b0d_advanceNumBits(&inflate->inputBuffer, numBitsUsed)) {
			inflate->status = INFLATE_NEED_INPUT;
			return false;
		}

		if (symbol >= 0 && symbol <= CODELEN_MAX_LIT) {
			// A literal codeword length
			codeLengthArray[i++] = (uint8_t)symbol;
		} else if (symbol == CODELEN_COPY) {
			// Copy the previous codeword length 3--6 times

			// 2 bits + 3
			if (!c49f5b0d_advanceNumBits(&inflate->inputBuffer, 2)) {
				inflate->status = INFLATE_NEED_INPUT;
				return false;
			}
			j = ((inflate->inputBuffer.bits & 0x03) + CODELEN_COPY_MIN);

			while (j--) {
				codeLengthArray[i] = codeLengthArray[i - 1];
				i++;
			}
		} else if (symbol == CODELEN_ZEROS) {
			// 3--10 zeros; 3 bits + 3
			if (!c49f5b0d_advanceNumBits(&inflate->inputBuffer, 3)) {
				inflate->status = INFLATE_NEED_INPUT;
				return false;
			}
			j = ((inflate->inputBuffer.bits & 0x07) + CODELEN_ZEROS_MIN);

			while (j--) {
				codeLengthArray[i++] = 0;
			}
		} else if (symbol == CODELEN_ZEROS2) {
			// 11--138 zeros; 7 bits + 138
			if (!c49f5b0d_advanceNumBits(&inflate->inputBuffer, 7)) {
				inflate->status = INFLATE_NEED_INPUT;
				return false;
			}
			j = ((inflate->inputBuffer.bits & 0x7F) + CODELEN_ZEROS2_MIN);

			while (j--) {
				codeLengthArray[i++] = 0;
			}
		} else {
			// Invalid symbol
			inflate->status = INFLATE_INPUT_ERROR;
			return false;
		}
	}

	// Initialize the litlen decoder
	f173ab5a_initHuffmanDecoder(litlenDecoder, &codeLengthArray[0], numLitlenLens);

	// Initialize the dist decoder
	f173ab5a_initHuffmanDecoder(distDecoder, &codeLengthArray[numLitlenLens], numDistLens);

	return true;
}

static bool processLiteralBackrefBlock(Inflate *inflate, HuffmanDecoder *litlenDecoder, HuffmanDecoder *distDecoder) {
	uint8_t *outputBuf;
	size_t numBitsUsed, totalBitsUsed, dist, length;
	int litlen, distSymbol;
	uint16_t extraBits;

	outputBuf = (uint8_t *) inflate->outputBuffer.currentBuf;

	while (true) {
		// Read a litlen symbol
		c49f5b0d_getNextBits(&inflate->inputBuffer);
		litlen = f173ab5a_huffmanDecode(&inflate->inputBuffer, litlenDecoder, &numBitsUsed);
		inflate->inputBuffer.bits >>= numBitsUsed;
		totalBitsUsed = numBitsUsed;

		if (litlen < 0 || litlen > LITLEN_MAX) {
			// Failed to decode, or invalid symbol
			inflate->status = INFLATE_INPUT_ERROR;
			return false;
		} else if (litlen <= UINT8_MAX) {
			// Literal
			if (!c49f5b0d_advanceNumBits(&inflate->inputBuffer, totalBitsUsed)) {
				inflate->status = INFLATE_NEED_INPUT;
				return false;
			}

			if (inflate->outputBuffer.length == INFLATE_OUTPUT_LENGTH) {
				inflate->status = INFLATE_OUTPUT_FULL;
				return false;
			}

			outputBuf[inflate->outputBuffer.length++] = (uint8_t)litlen;
			continue;
		} else if (litlen == LITLEN_EOB) {
			// End of block
			if (!c49f5b0d_advanceNumBits(&inflate->inputBuffer, totalBitsUsed)) {
				inflate->status = INFLATE_NEED_INPUT;
				return false;
			}

			inflate->status = INFLATE_SUCCESS;
			return true;
		}

		// It is a back reference so figure out the length
		length = litlen_tbl[litlen - LITLEN_TBL_OFFSET].baseLength;
		extraBits = litlen_tbl[litlen - LITLEN_TBL_OFFSET].extraBits;
		if (extraBits != 0) {
			// length += lsb(bits, extraBits);
			length += (inflate->inputBuffer.bits & ((1UL << extraBits) - 1));
			inflate->inputBuffer.bits >>= extraBits;
			totalBitsUsed += extraBits;
		}

		// Get the distance
		distSymbol = f173ab5a_huffmanDecode(&inflate->inputBuffer, distDecoder, &numBitsUsed);
		inflate->inputBuffer.bits >>= numBitsUsed;
		totalBitsUsed += numBitsUsed;

		if (distSymbol < 0 || distSymbol > DISTSYM_MAX) {
			// Failed to decode, or invalid symbol
			inflate->status = INFLATE_INPUT_ERROR;
			return false;
		}

		dist = dist_tbl[distSymbol].baseDist;
		extraBits = dist_tbl[distSymbol].extraBits;

		if (extraBits != 0) {
			// dist += lsb(bits, extraBits);
			dist += (inflate->inputBuffer.bits & ((1UL << extraBits) - 1));
			inflate->inputBuffer.bits >>= extraBits;
			totalBitsUsed += extraBits;
		}

		if (!c49f5b0d_advanceNumBits(&inflate->inputBuffer, totalBitsUsed)) {
			inflate->status = INFLATE_NEED_INPUT;
			return false;
		}

		// Bounds check and output the backref
		if (dist > inflate->outputBuffer.length) {
			inflate->status = INFLATE_INPUT_ERROR;
			return false;
		}

		if (round_up(length, 8) <= INFLATE_OUTPUT_LENGTH - inflate->outputBuffer.length) {
			output_backref64(&inflate->outputBuffer, dist, length);
		} else if (length <= INFLATE_OUTPUT_LENGTH - inflate->outputBuffer.length) {
			lz77_output_backref(&inflate->outputBuffer, dist, length);
		} else {
			inflate->status = INFLATE_OUTPUT_FULL;
			return false;
		}

		inflate->outputBuffer.length += length;
	}
}

/*
 * Output the (dist,len) backref at dst_pos in dst using 64-bit wide writes.
 * There must be enough room for len bytes rounded to the next multiple of 8.
 */
static void output_backref64(OutputBuffer *outputBuffer, size_t dist, size_t length) {
	uint8_t *outputBuf;
	uint8_t *backref;
//	size_t i;
//	uint64_t tmp;

	if (length > dist) {
		// Self-overlapping backref; fall back to byte-by-byte copy
		lz77_output_backref(outputBuffer, dist, length);
		return;
	}

//	length >>= 3;
	outputBuf = (uint8_t *)outputBuffer->currentBuf + outputBuffer->length;
	backref = outputBuf - dist;
	f668c4bd_memcopy(backref, outputBuf, length);
/*
	i = 0;
	do {
		memcpy(&tmp, &dst[dst_pos - dist + i], 8);
		memcpy(&dst[dst_pos + i], &tmp, 8);
		i += 8;
	} while (i < len);
*/
}

/*
 * Output the (dist,len) backref at dst_pos in dst.
 */
static void lz77_output_backref(OutputBuffer *outputBuffer, size_t dist, size_t length) {
	uint8_t *outputBuf;
	uint8_t *backref;

	outputBuf = (uint8_t *)outputBuffer->currentBuf + outputBuffer->length;
	backref = outputBuf - dist;

	for (uint32_t i=0; i < length; i++) {
		*outputBuf = *backref;
		outputBuf++;
		backref++;
	}
}
