;
; bits.linux.asm - DevOpsBroker NASM file for bit-manipulation functionality
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
; Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-51
;
; This file implements the following x86-64 assembly language functions for the
; org.devopsbroker.compress.bits.h header file:
;
;   o uint16_t e474415b_reverse16(uint16_t value, uint32_t numBits);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════


; ═════════════════════════════ Initialized Data ═════════════════════════════

section .data               ; DX directives
 align 4

reverseBitLUT:    db 0x00,0x20,0x10,0x30,0x08,0x28,0x18,0x38,\
                     0x04,0x24,0x14,0x34,0x0c,0x2c,0x1c,0x3c,\
					 0x02,0x22,0x12,0x32,0x0a,0x2a,0x1a,0x3a,\
					 0x06,0x26,0x16,0x36,0x0e,0x2e,0x1e,0x3e,\
					 0x01,0x21,0x11,0x31,0x09,0x29,0x19,0x39,\
					 0x05,0x25,0x15,0x35,0x0d,0x2d,0x1d,0x3d,\
					 0x03,0x23,0x13,0x33,0x0b,0x2b,0x1b,0x3b,\
					 0x07,0x27,0x17,0x37,0x0f,0x2f,0x1f,0x3f

; ════════════════════════════ Uninitialized Data ════════════════════════════

section .bss                ; RESX directives

; ══════════════════════════════ Assembly Code ═══════════════════════════════

section .text               ; TEXT section

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ e474415b_reverse16 ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  e474415b_reverse16:function
e474415b_reverse16:
; Parameters:
;	rdi : uint16_t value
;	rsi : uint32_t numBits
; Local Variables:
;	edx : index variable
;	 cx : temp variable
;	r10 : lookup table address

.prologue:                            ; functions typically have a prologue
	prefetcht0 [rel reverseBitLUT]    ; prefetch lookup table into the CPU cache
	lea        r10,[rel reverseBitLUT]
	xor        ax, ax                 ; return value = 0

.reverseBits:
	xor        cx, cx                 ; temp value = 0
	movzx      edx, di                ; edx = value & 0x3F
	and        edx, 0x3F

	mov        cl, [r10+rdx]          ; reverse = lut[index]
	shl        cx, 10                 ; reverse = (cx << 10)
	mov        ax, cx

	shr        di, 6                  ; value >>= 6
	test       di, di                 ; if (value == 0)
	jz         .adjustNumBits

	xor        cx, cx                 ; temp value = 0
	movzx      edx, di                ; edx = value & 0x3F
	and        edx, 0x3F

	mov        cl, [r10+rdx]          ; reverse = lut[index]
	shl        cx, 4                  ; reverse |= (cx <<= 4)
	or         ax, cx

	shr        di, 6                  ; value >>= 6
	test       di, di                 ; if (value == 0)
	jz         .adjustNumBits

	xor        cx, cx                 ; temp value = 0
	movzx      edx, di                ; edx = value

	mov        cl, [r10+rdx]          ; reverse = lut[index]
	shr        cl, 2
	or         ax, cx                 ; reverse |= (cl >> 2)

.adjustNumBits:
	mov        cl, 0x10               ; adjust = (16 - numBits)
	sub        cl, sil
	shr        ax, cl                 ; reverse >>= adjust;

.epilogue:                            ; functions typically have an epilogue
	ret                               ; pop return address from stack and jump there
