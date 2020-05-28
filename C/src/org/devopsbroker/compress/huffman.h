/*
 * huffman.h - DevOpsBroker C header file for Huffman-related functionality
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
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-53
 *
 * echo ORG_DEVOPSBROKER_COMPRESS_HUFFMAN | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_COMPRESS_HUFFMAN_H
#define ORG_DEVOPSBROKER_COMPRESS_HUFFMAN_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>

#include <assert.h>

#include "iobuffer.h"

#include "../lang/memory.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define MAX_HUFFMAN_BITS  15               // Deflate uses max 15-bit codewords
#define MAX_HUFFMAN_SYMBOLS  288           // Deflate uses max 288 symbols
#define HUFFMAN_LOOKUP_TABLE_BITS  8       // Seems a good trade-off

// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct HuffmanDecoder {
	// Lookup table for fast decoding of short codewords
	struct {
		uint16_t sym : 9;              // Wide enough to fit the max symbol nbr
		uint16_t len : 7;              // 0 means no symbol
	} table[1U << HUFFMAN_LOOKUP_TABLE_BITS];

	// "Sentinel bits" value for each codeword length
	uint16_t sentinel_bits[MAX_HUFFMAN_BITS + 1];

	// First symbol index minus first codeword mod 2**16 for each length
	uint16_t offset_first_sym_idx[MAX_HUFFMAN_BITS + 1];

	// Map from symbol index to symbol
	uint16_t syms[MAX_HUFFMAN_SYMBOLS];

	uint64_t numSymbols;
} HuffmanDecoder;

static_assert(sizeof(HuffmanDecoder) == 1160, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f173ab5a_createHuffmanDecoder
 * Description: Creates a HuffmanDecoder struct instance
 *
 * Parameters:
 *   lengthArray    The array of lengths to use to initialize the decoder
 *   numElements    The number of elements in the length array
 * Returns:     A HuffmanDecoder struct instance
 * ----------------------------------------------------------------------------
 */
HuffmanDecoder *f173ab5a_createHuffmanDecoder(uint8_t *lengthArray, uint32_t numElements);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f173ab5a_destroyHuffmanDecoder
 * Description: Frees the memory allocated to the HuffmanDecoder struct pointer
 *
 * Parameters:
 *   decoder    A pointer to the HuffmanDecoder instance to destroy
 * ----------------------------------------------------------------------------
 */
void f173ab5a_destroyHuffmanDecoder(HuffmanDecoder *decoder);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f173ab5a_initHuffmanDecoder
 * Description: Initializes an existing HuffmanDecoder struct
 *
 * Parameters:
 *   decoder        A pointer to the HuffmanDecoder instance to initalize
 *   lengthArray    The array of lengths to use to initialize the decoder
 *   numElements    The number of elements in the length array
 * ----------------------------------------------------------------------------
 */
void f173ab5a_initHuffmanDecoder(HuffmanDecoder *decoder, uint8_t *lengthArray, uint32_t numElements);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f173ab5a_huffmanDecode
 * Description: Use the decoder d to decode a symbol from the LSB-first
 *              zero-padded bits. Returns the decoded symbol number or -1 if
 *              no symbol could be decoded. *num_used_bits will be set to the
 *              number of bits used to decode the symbol, or zero if no symbol
 *              could be decoded.
 *
 * Parameters:
 *   inputBuffer    The input buffer instance
 *   decoder        A pointer to the HuffmanDecoder instance to initalize
 *   numBitsUsed    The number of bits used during Huffman decoding
 * ----------------------------------------------------------------------------
 */
int f173ab5a_huffmanDecode(InputBuffer *inputBuffer, HuffmanDecoder *decoder, size_t *numBitsUsed);

#endif /* ORG_DEVOPSBROKER_COMPRESS_HUFFMAN_H */
