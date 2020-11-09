;
; integer.linux.asm - DevOpsBroker NASM file for integer-related functionality
;
; Copyright (C) 2019 Edward Smith <edwardsmith@devopsbroker.org>
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
; Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-21
;
; This file implements the following x86-64 assembly language functions for the
; org.devopsbroker.lang.integer.h header file:
;
;   o int f45efac2_parseInt(const char *source, int *value);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════

; Constants
%define ERROR_CODE    -1

; character values
%define NEGATIVE    0x2D
%define ZERO        0x30
%define NINE        0x39

; ═════════════════════════════ Initialized Data ═════════════════════════════

section .data               ; DX directives

; ════════════════════════════ Uninitialized Data ════════════════════════════

section .bss                ; RESX directives

; ══════════════════════════════ Assembly Code ═══════════════════════════════

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f45efac2_parseInt ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f45efac2_parseInt:function
	section .text
f45efac2_parseInt:
; Parameters:
;	rdi : char *source
;   rsi ; int *value
; Local Variables:
;	eax  : return value
;	dx   : numChars(dh), isNegative(dl)
;	r8   : 64-bit character buffer

.prologue:                            ; functions typically have a prologue
	mov        r8, [rdi]              ; put first eight characters into r8
	xor        eax, eax               ; clear return value
	xor        dx, dx                 ; numChars(dh) = 0, isNegative(dl) = false

	cmp        r8b, NEGATIVE           ; if (ch == '-')
	jne        .positiveInt

	mov        dl, r8b                ; isNegative = true
	inc        dh                     ; numChars++
	shr        r8, 8                  ; shift to the next character

.positiveInt:
	test       r8b, r8b
	jz         .invalidInt

	; Calculate the integer
.whileInteger:
	cmp        r8b, NINE
	jg         .invalidInt            ; Error if character greater than nine
	sub        r8b, ZERO
	jl         .invalidInt            ; Error if character less than zero

	movzx      r9d, r8b               ; eax = (eax * 10) + r8b
	lea        eax, [eax+eax*4]
	lea        eax, [r9d+eax*2]

	inc        dh                     ; numChars++
	shr        r8, 8                  ; shift to the next character

	cmp        dh, 0x08               ; if (numChars == 8)
	jne        .testWhile

	add        rdi, 0x08              ; load next eight characters into r8
	mov        r8, [rdi]

	; while (ch && numChars < 11)
.testWhile:
	test       r8b, r8b               ; if (ch == '\0')
	jz         .isNegative

	cmp        dh, 0x0B               ; if (numChars == 11)
	jne        .whileInteger
	; Error if we already processed eleven characters

.invalidInt:
	mov        eax, ERROR_CODE        ; Set return value to ERROR_CODE
	ret                               ; pop return address from stack and jump there

.isNegative:
	test       dl, dl                 ; if (isNegative)
	jz         .epilogue

	neg        eax                    ; eax = -eax

.epilogue:                            ; functions typically have an epilogue
	mov        [rsi], eax             ; set value parameter
	xor        eax, eax               ; clear return value
	ret                               ; pop return address from stack and jump there
