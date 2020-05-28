/*
 * huffman.c - DevOpsBroker C source file for Huffman-related functionality
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

#include "huffman.h"
#include "bits.h"

#include "../lang/error.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static void table_insert(HuffmanDecoder *decoder, uint32_t symbol, uint32_t length, uint16_t codeword);

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

HuffmanDecoder *f173ab5a_createHuffmanDecoder(uint8_t *lengthArray, uint32_t numElements) {
	HuffmanDecoder *decoder = f668c4bd_malloc(sizeof(HuffmanDecoder));

	f173ab5a_initHuffmanDecoder(decoder, lengthArray, numElements);

	return decoder;
}

void f173ab5a_destroyHuffmanDecoder(HuffmanDecoder *decoder) {
	f668c4bd_free(decoder);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void f173ab5a_initHuffmanDecoder(HuffmanDecoder *decoder, uint8_t *lengthArray, uint32_t numElements) {
	uint16_t next_code[16] = {0};
	uint16_t sym_idx[16] = {0};
	uint8_t count[16] = {0};
	uint32_t i, l, s;

	// Keep track of the number of symbols
	decoder->numSymbols = numElements;

	// Initialize the HuffmanDecoder memory to zeroes
	f668c4bd_meminit(decoder, sizeof(HuffmanDecoder));

	// Count the number of codes for each code length
	for (i=0; i < numElements; i++) {
		count[lengthArray[i]]++;
	}

//	printf("Count codewords lengths: ");
//	d9352b73_printByteArray(count, 16);
//	printf("\n");

	// Find the numerical value of the smallest code for each code length
	count[0] = 0;
//	printf("Sentinal bits: \n");
	for (i=0, l=1; l <= MAX_HUFFMAN_BITS; i++, l++) {
		next_code[l] = (uint16_t) ((next_code[i] + count[i]) << 1);

		s = (uint32_t)((next_code[l] + count[l]) << (MAX_HUFFMAN_BITS - l));
		decoder->sentinel_bits[l] = (uint16_t)s;
		sym_idx[l] = sym_idx[i] + count[i];
		decoder->offset_first_sym_idx[l] = sym_idx[l] - next_code[l];

//		printf("\tLength: %d  Code[l]: %u  Count[l]: %u  Sentinal Bits: %u  Sym_idx[l]: %u  Offset[l]: %u\n", l, next_code[l], count[l], s, sym_idx[l], decoder->offset_first_sym_idx[l]);
	}
//	printf("\n");

//	printf("Codes for the codelengths: ");
//	a4b6dbea_printShortArray(next_code, MAX_HUFFMAN_BITS + 1);
//	printf("\n");

//	printf("Symbol indexes: ");
//	a4b6dbea_printShortArray(sym_idx, MAX_HUFFMAN_BITS + 1);
//	printf("\n");

	// Build mapping from index to symbol and populate the lookup table
//	printf("Symbols: \n");
	for (i = 0; i < numElements; i++) {
		l = lengthArray[i];
		if (l == 0) {
			continue;
		}

		decoder->syms[sym_idx[l]] = (uint16_t)i;
//		printf("\tIndex: %d  Length[i]: %u  Syms[sym_idx[l]]: %u  Code[l]: %u\n", i, l, decoder->syms[sym_idx[l]], next_code[l]);
		sym_idx[l]++;

		if (l <= HUFFMAN_LOOKUP_TABLE_BITS) {
			table_insert(decoder, i, l, next_code[l]);
			next_code[l]++;
		}
	}
//	printf("\n");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int f173ab5a_huffmanDecode(InputBuffer *inputBuffer, HuffmanDecoder *decoder, size_t *numBitsUsed) {
	uint16_t lookupBits;
	uint16_t bits;
	uint32_t l;
	uint32_t symbolIndex;

	bits = (uint16_t) inputBuffer->bits;

	// First try the lookup table
	lookupBits = bits & 0xFF;
	if (decoder->table[lookupBits].len != 0) {
		(*numBitsUsed) = decoder->table[lookupBits].len;
		return decoder->table[lookupBits].sym;
	}

	// Then do canonical decoding with the bits in MSB-first order
	bits = e474415b_reverse16(bits, MAX_HUFFMAN_BITS);
	for (l = HUFFMAN_LOOKUP_TABLE_BITS + 1; l <= MAX_HUFFMAN_BITS; l++) {
		if (bits < decoder->sentinel_bits[l]) {
			bits >>= MAX_HUFFMAN_BITS - l;

			symbolIndex = (uint16_t)(decoder->offset_first_sym_idx[l] + bits);
			(*numBitsUsed) = l;

			return decoder->syms[symbolIndex];
		}
	}

	return SYSTEM_ERROR_CODE;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static void table_insert(HuffmanDecoder *decoder, uint32_t symbol, uint32_t length, uint16_t codeword) {
	int padLength;
	uint16_t padding, index;

	// Make the codeword LSB-first
	codeword = e474415b_reverse16(codeword, length);
	padLength = (1U << (HUFFMAN_LOOKUP_TABLE_BITS - length));

	// Pad the pad_len upper bits with all bit combinations
	for (padding = 0; padding < padLength; padding++) {
		index = (uint16_t)(codeword | (padding << length));
		decoder->table[index].sym = (uint16_t)symbol;
		decoder->table[index].len = (uint16_t)length;
	}
}
