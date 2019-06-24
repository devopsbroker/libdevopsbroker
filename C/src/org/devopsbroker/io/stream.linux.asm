;
; stream.linux.asm - DevOpsBroker NASM file for stream-related functionality
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
; Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-22
;
; This file implements the following x86-64 assembly language functions for the
; org.devopsbroker.io.stream.h header file:
;
;   o void a8d15ebe_ungetString(char *string, FILE *stream);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════


; ═════════════════════════════ Initialized Data ═════════════════════════════

section .data               ; DX directives

; ════════════════════════════ Uninitialized Data ════════════════════════════

section .bss                ; RESX directives

; ══════════════════════════════ Assembly Code ═══════════════════════════════

section .text               ; TEXT section

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ External Resources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

extern  ungetc

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~ a8d15ebe_ungetString ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  a8d15ebe_ungetString:function
a8d15ebe_ungetString:
; Parameters:
;	rdi : char *string
;	rsi : FILE *stream
; Local Variables:
;	ecx : int strLen
;	r8b : character variable

.prologue:                            ; functions typically have a prologue
	test       rdi, rdi               ; if (string == NULL)
	jz         .epilogue

	prefetcht0 [rdi]                  ; prefetch string into the CPU cache
	xor        ecx, ecx               ; strLen = 0
	push       r12                    ; save r12 and r13 onto the stack
	push       r13
	push       r14

.whileString:
	mov        r8b, [rdi]             ; if (ch == '\0')
	test       r8b, r8b
	jz         .foundNull

	inc        rdi
	inc        ecx
	jmp        .whileString

.foundNull:
	test       ecx, ecx               ; if (strLen > 0)
	jz         .epilogue

	mov        r12, rdi               ; r12 = char *string
	mov        r13d, ecx              ; r13d = int strLen
	mov        r14, rsi               ; r14 = FILE *stream

.ungetString:
	dec        r12
	dec        r13d

	mov        r8b, [r12]             ; edi = character
	movzx      edi, r8b
	mov        rsi, r14               ; rsi = FILE *stream
	call       ungetc WRT ..plt

	test       r13d, r13d             ; if (strLen > 0)
	jnz        .ungetString

.epilogue:                            ; functions typically have an epilogue
	pop        r14                    ; retrieve r12, r13, and r14 from the stack
	pop        r13
	pop        r12
	ret                               ; pop return address from stack and jump there
