/*
 * md5.c - DevOpsBroker C source file for the MD5 hash functionality
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
 *
 * -----------------------------------------------------------------------------
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-59
 *
 * I am calling this **THE** MD5 hash reference implementation as per the RFC 1321
 * specifications. I copied the macros, MD5Transform, and MDPrint methods directly
 * from the RFC 1321 specification. They are left inside here commented out in
 * case anyone is in doubt about the reference outputs.
 *
 * I then took the C language reference implementation and did it in x86-64
 * assembly language. If you doubt the assembly code, then uncomment the C
 * code and do a compare/contrast. The functionality is equivalent.
 *
 * I was going to copy the GNU coreutils algorithm, even though it is not to
 * spec, since they are on every Linux box. But I immediately ran into issues
 * after cloning the GitHub coreutils project as I could not find the md5.h
 * header file. In fact, I have no fucking idea what the GNU developers are
 * doing since they should not be able to even compile md5sum without the
 * header file.
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include "md5.h"

#include "../lang/error.h"
#include "../lang/memory.h"
#include "../lang/stringbuilder.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define A_INIT_VALUE   0x67452301
#define B_INIT_VALUE   0xefcdab89
#define C_INIT_VALUE   0x98badcfe
#define D_INIT_VALUE   0x10325476

#define ONE_BIT_VALUE  0x80
/*
// F, G, H and I are basic MD5 functions
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// ROTATE_LEFT rotates x left n bits
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4. Rotation is
// separate from addition to prevent recomputation.
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (uint32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}

#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}
*/
// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

extern void f1518caf_md5Transform(uint32_t *state, void *block);

//static void MD5Transform(uint32_t *state, unsigned char *block);

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void f1518caf_initMD5State(uint32_t *state) {
	state[0] = A_INIT_VALUE;
	state[1] = B_INIT_VALUE;
	state[2] = C_INIT_VALUE;
	state[3] = D_INIT_VALUE;
}

void f1518caf_md5(uint32_t *state, void *buffer, uint32_t length) {
	uint8_t message[64];
	uint64_t msgLength;

	// Save original message length for later
	msgLength = length;

	while (length > 64) {
		f1518caf_md5Transform(state, buffer);
		length -= 64;
	}

	if (length < 56) {
		f668c4bd_memcopy(buffer, message, length);
		message[length++] = ONE_BIT_VALUE;
		f668c4bd_meminit(message + length, 64 - length);

		((uint64_t*) message)[7] = msgLength;

		f1518caf_md5Transform(state, message);

	} else if (length < 64) {
		f668c4bd_memcopy(buffer, message, length);
		message[length++] = ONE_BIT_VALUE;

		for (; length < 64; length++) {
			message[length] = 0;
		}

		f1518caf_md5Transform(state, message);

		f668c4bd_meminit(message, 56);
		((uint64_t*) message)[7] = msgLength;
		f1518caf_md5Transform(state, message);
	} else {
		f1518caf_md5Transform(state, buffer);

		f668c4bd_meminit(message, 56);
		message[0] = ONE_BIT_VALUE;
		((uint64_t*) message)[7] = msgLength;
		f1518caf_md5Transform(state, message);
	}
}

void f1518caf_md5Rounds(uint32_t *state, uint32_t numRounds) {
	uint8_t message[64] = {
		0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	((uint64_t*) message)[7] = 16UL;

	while (numRounds > 0) {
		f668c4bd_memcopy(state, message, 16);
		f1518caf_md5Transform(state, message);
		numRounds--;
	}
}

void f1518caf_md5Stream(uint32_t *state, void *buffer, uint32_t length) {

	// Exit if buffer length is not a multiple of 64
	if ((length & 0x3F) != 0) {
		StringBuilder errorMessage;

		c598a24c_initStringBuilder(&errorMessage);
		c598a24c_append_string(&errorMessage, "f1518caf_md5Stream(): Invalid buffer length of ");
		c598a24c_append_uint(&errorMessage, length);

		c7c88e52_printError_string(errorMessage.buffer);
		c598a24c_cleanUpStringBuilder(&errorMessage);
		exit(EXIT_FAILURE);
	}

	// Calculate the MD5 based upon the stream of bytes
	do {
		f1518caf_md5Transform(state, buffer);
		length -= 64;
	} while (length > 0);
}

void f1518caf_md5StreamEnd(uint32_t *state, void *buffer, uint32_t length, size_t origLen) {
	uint8_t message[64];

	while (length > 64) {
		f1518caf_md5Transform(state, buffer);
		length -= 64;
	}

	if (length < 56) {
		f668c4bd_memcopy(buffer, message, length);
		message[length++] = ONE_BIT_VALUE;
		f668c4bd_meminit(message + length, 64 - length);

		((uint64_t*) message)[7] = origLen;

		f1518caf_md5Transform(state, message);

	} else if (length < 64) {
		f668c4bd_memcopy(buffer, message, length);
		message[length++] = ONE_BIT_VALUE;

		for (; length < 64; length++) {
			message[length] = 0;
		}

		f1518caf_md5Transform(state, message);

		f668c4bd_meminit(message, 56);
		((uint64_t*) message)[7] = origLen;
		f1518caf_md5Transform(state, message);
	} else {
		f1518caf_md5Transform(state, buffer);

		f668c4bd_meminit(message, 56);
		message[0] = ONE_BIT_VALUE;
		((uint64_t*) message)[7] = origLen;
		f1518caf_md5Transform(state, message);
	}
}

void f1518caf_md5WithSalt(uint32_t *state, uint8_t *salt, uint32_t saltLen, void *buffer, uint32_t length) {

	// Exit if salt length is greater than 64
	if (saltLen > 64) {
		StringBuilder errorMessage;

		c598a24c_initStringBuilder(&errorMessage);
		c598a24c_append_string(&errorMessage, "f1518caf_md5WithSalt(): Invalid salt length of ");
		c598a24c_append_uint(&errorMessage, saltLen);

		c7c88e52_printError_string(errorMessage.buffer);
		c598a24c_cleanUpStringBuilder(&errorMessage);
		exit(EXIT_FAILURE);
	}

	// 1. Compute the MD5 digest of the salt value
	f1518caf_md5(state, salt, saltLen);

	// 2. Then compute the MD5 digest of the buffer
	f1518caf_md5(state, buffer, length);
}

void f1518caf_printMD5(uint32_t *digest) {
	for (uint32_t i=0; i < 16; i++) {
		printf ("%02x", ((uint8_t*)digest)[i]);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
static void MD5Transform(uint32_t *state, unsigned char *block) {
	uint32_t a, b, c, d;
	uint32_t x[16];

	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];

	// Decode from char* to uint32_t* values
	for (uint32_t i=0, j=0; j < 64; i++, j += 4) {
		x[i] = ((uint32_t) block[j])
		     | (((uint32_t) block[j+1]) << 8)
			 | (((uint32_t) block[j+2]) << 16)
			 | (((uint32_t) block[j+3]) << 24);
	}

	// Round 1
	FF(a, b, c, d, x[ 0], 7, 0xd76aa478); // 1
	FF(d, a, b, c, x[ 1], 12, 0xe8c7b756); // 2
	FF(c, d, a, b, x[ 2], 17, 0x242070db); // 3
	FF(b, c, d, a, x[ 3], 22, 0xc1bdceee); // 4
	FF(a, b, c, d, x[ 4], 7, 0xf57c0faf); // 5
	FF(d, a, b, c, x[ 5], 12, 0x4787c62a); // 6
	FF(c, d, a, b, x[ 6], 17, 0xa8304613); // 7
	FF(b, c, d, a, x[ 7], 22, 0xfd469501); // 8
	FF(a, b, c, d, x[ 8], 7, 0x698098d8); // 9
	FF(d, a, b, c, x[ 9], 12, 0x8b44f7af); // 10
	FF(c, d, a, b, x[10], 17, 0xffff5bb1); // 11
	FF(b, c, d, a, x[11], 22, 0x895cd7be); // 12
	FF(a, b, c, d, x[12], 7, 0x6b901122); // 13
	FF(d, a, b, c, x[13], 12, 0xfd987193); // 14
	FF(c, d, a, b, x[14], 17, 0xa679438e); // 15
	FF(b, c, d, a, x[15], 22, 0x49b40821); // 16

	// Round 2
	GG(a, b, c, d, x[ 1], 5, 0xf61e2562); // 17
	GG(d, a, b, c, x[ 6], 9, 0xc040b340); // 18
	GG(c, d, a, b, x[11], 14, 0x265e5a51); // 19
	GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); // 20
	GG(a, b, c, d, x[ 5], 5, 0xd62f105d); // 21
	GG(d, a, b, c, x[10], 9,  0x2441453); // 22
	GG(c, d, a, b, x[15], 14, 0xd8a1e681); // 23
	GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); // 24
	GG(a, b, c, d, x[ 9], 5, 0x21e1cde6); // 25
	GG(d, a, b, c, x[14], 9, 0xc33707d6); // 26
	GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); // 27
	GG(b, c, d, a, x[ 8], 20, 0x455a14ed); // 28
	GG(a, b, c, d, x[13], 5, 0xa9e3e905); // 29
	GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8); // 30
	GG(c, d, a, b, x[ 7], 14, 0x676f02d9); // 31
	GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); // 32

	// Round 3
	HH(a, b, c, d, x[ 5], 4, 0xfffa3942); // 33
	HH(d, a, b, c, x[ 8], 11, 0x8771f681); // 34
	HH(c, d, a, b, x[11], 16, 0x6d9d6122); // 35
	HH(b, c, d, a, x[14], 23, 0xfde5380c); // 36
	HH(a, b, c, d, x[ 1], 4, 0xa4beea44); // 37
	HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9); // 38
	HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); // 39
	HH(b, c, d, a, x[10], 23, 0xbebfbc70); // 40
	HH(a, b, c, d, x[13], 4, 0x289b7ec6); // 41
	HH(d, a, b, c, x[ 0], 11, 0xeaa127fa); // 42
	HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); // 43
	HH(b, c, d, a, x[ 6], 23,  0x4881d05); // 44
	HH(a, b, c, d, x[ 9], 4, 0xd9d4d039); // 45
	HH(d, a, b, c, x[12], 11, 0xe6db99e5); // 46
	HH(c, d, a, b, x[15], 16, 0x1fa27cf8); // 47
	HH(b, c, d, a, x[ 2], 23, 0xc4ac5665); // 48

	// Round 4
	II(a, b, c, d, x[ 0], 6, 0xf4292244); // 49
	II(d, a, b, c, x[ 7], 10, 0x432aff97); // 50
	II(c, d, a, b, x[14], 15, 0xab9423a7); // 51
	II(b, c, d, a, x[ 5], 21, 0xfc93a039); // 52
	II(a, b, c, d, x[12], 6, 0x655b59c3); // 53
	II(d, a, b, c, x[ 3], 10, 0x8f0ccc92); // 54
	II(c, d, a, b, x[10], 15, 0xffeff47d); // 55
	II(b, c, d, a, x[ 1], 21, 0x85845dd1); // 56
	II(a, b, c, d, x[ 8], 6, 0x6fa87e4f); // 57
	II(d, a, b, c, x[15], 10, 0xfe2ce6e0); // 58
	II(c, d, a, b, x[ 6], 15, 0xa3014314); // 59
	II(b, c, d, a, x[13], 21, 0x4e0811a1); // 60
	II(a, b, c, d, x[ 4], 6, 0xf7537e82); // 61
	II(d, a, b, c, x[11], 10, 0xbd3af235); // 62
	II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); // 63
	II(b, c, d, a, x[ 9], 21, 0xeb86d391); // 64

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}
*/
