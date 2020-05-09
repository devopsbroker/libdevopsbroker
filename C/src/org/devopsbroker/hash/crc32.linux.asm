;
; crc32.linux.asm - DevOpsBroker NASM file for CRC-32 hash functionality
;
; Copyright (C) 2020 Edward Smith <edwardsmith@devopsbroker.org>
;
; This program is free software: you can redistribute it and/or modify it under
; the terms of the GNU General Public License as published by the Free Software
; Foundation, either version 3 of the License, or (at your option) any later
; version.
;
; This program is distributed in the hope that it will be useful, but WITHOUT
; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
; FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
; details.
;
; You should have received a copy of the GNU General Public License along with
; this program.  If not, see <http://www.gnu.org/licenses/>.
;
; -----------------------------------------------------------------------------
; Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-46
;
; This file implements the following x86-64 assembly language functions for the
; org.devopsbroker.lang.string.h header file:
;
;   o void b7e0468d_crc32(void *buffer, uint32_t length);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════


; ═════════════════════════════ Initialized Data ═════════════════════════════

section .data               ; DX directives
 align 4

halfByteLUT:    dd 0x00000000,0x1DB71064,0x3B6E20C8,0x26D930AC,\
				   0x76DC4190,0x6B6B51F4,0x4DB26158,0x5005713C,\
				   0xEDB88320,0xF00F9344,0xD6D6A3E8,0xCB61B38C,\
				   0x9B64C2B0,0x86D3D2D4,0xA00AE278,0xBDBDF21C

; ════════════════════════════ Uninitialized Data ════════════════════════════

section .bss                ; RESX directives

; ══════════════════════════════ Assembly Code ═══════════════════════════════

section .text               ; TEXT section

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ b7e0468d_crc32 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  b7e0468d_crc32:function
b7e0468d_crc32:
; Parameters:
;	rdi : void *buffer
;	esi : uint32_t length
;	edx : uint32_t crc32
; Local Variables:
;	eax : CRC-32 return value
;	 dl : loop counter
;	rcx : 64-bit data buffer
;	r8d : lookup table index
;	r9d : current byte
;	r10 : lookup table address
;	r11 : lookup table value

.prologue:                            ; functions typically have a prologue
	prefetcht0 [rel halfByteLUT]      ; prefetch lookup table into the CPU cache
	prefetcht0 [rdi]                  ; prefetch buffer into the CPU cache
	mov        eax, edx               ; return value = crc32 parameter
	not        eax                    ; perform one's complement on crc32 parameter

	lea        r10,[rel halfByteLUT]  ; r10 = starting address of lookup table
	mov        rcx, [rdi]             ; load first eight buffer bytes into rcx
	lea        rdi, [rdi+8]           ; buffer += 8
	mov        dl, 0x08               ; loopCounter = 8

	cmp        esi, 0x08              ; if (length < 8)
	jb         .bytes

.eightBytes:
	; crc = lut[(crc ^ *current) & 0x0F] ^ (crc >> 4);
	mov        r8d, eax               ; index = (crc ^ *current) & 0x0F
	movzx      r9d, cl
	xor        r8d, r9d
	and        r8d, 0x0F

	mov        r11d,[r10+4*r8]        ; crc32 = lut[index] ^ (crc >> 4)
	shr        eax, 4
	xor        eax, r11d

	; crc = lut[(crc ^ (*current >> 4)) & 0x0F] ^ (crc >> 4);
	shr        cl, 4                  ; index = (crc ^ (*current >> 4)) & 0x0F
	mov        r8d, eax
	movzx      r9d, cl
	xor        r8d, r9d
	and        r8d, 0x0F

	mov        r11d,[r10+4*r8]        ; crc32 = lut[index] ^ (crc >> 4)
	shr        eax, 4
	xor        eax, r11d

	shr        rcx, 8                 ; move to next byte
	dec        dl                     ; loopCounter--
	jnz        .eightBytes

.manageEightByteBuffer:
	mov        rcx, [rdi]             ; load next eight buffer bytes into rcx
	lea        rdi, [rdi+8]           ; buffer += 8
	mov        dl, 0x08               ; loopCounter = 8

	sub        esi, 0x08              ; length -= 8
	cmp        esi, 0x08              ; if (length >= 8)
	jae        .eightBytes

.bytes:
	test       sil, sil               ; if (numBytes == 0)
	jz         .epilogue

.whileBytes:
	; crc = lut[(crc ^ *current) & 0x0F] ^ (crc >> 4);
	mov        r8d, eax               ; index = (crc ^ *current) & 0x0F
	movzx      r9d, cl
	xor        r8d, r9d
	and        r8d, 0x0F

	mov        r11d,[r10+4*r8]        ; crc32 = lut[index] ^ (crc >> 4)
	shr        eax, 4
	xor        eax, r11d

	; crc = lut[(crc ^ (*current >> 4)) & 0x0F] ^ (crc >> 4);
	shr        cl, 4                  ; index = (crc ^ (*current >> 4)) & 0x0F
	mov        r8d, eax
	movzx      r9d, cl
	xor        r8d, r9d
	and        r8d, 0x0F

	mov        r11d,[r10+4*r8]        ; crc32 = lut[index] ^ (crc >> 4)
	shr        eax, 4
	xor        eax, r11d

	shr        rcx, 8                 ; move to next byte
	dec        sil                    ; length--
	jnz        .whileBytes

.epilogue:                            ; functions typically have an epilogue
	not        eax                    ; return the one's complement of the CRC-32 value
	ret                               ; pop return address from stack and jump there
