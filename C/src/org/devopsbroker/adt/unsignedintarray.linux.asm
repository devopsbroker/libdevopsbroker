;
; unsignedintarray.linux.asm - DevOpsBroker NASM file for the UnsignedIntArray struct
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
; Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-40
;
; This file defines the following x86-64 assembly language functions for the
; UnsignedIntArray struct:
;
;   o void a8638224_add(UnsignedIntArray *listArray, uint32_t element);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════

; Constants
%define SIZEOF_UINT    0x04

; ═════════════════════════════ Initialized Data ═════════════════════════════

section .data               ; DX directives

; ════════════════════════════ Uninitialized Data ════════════════════════════

section .bss                ; RESX directives

; ══════════════════════════════ Assembly Code ═══════════════════════════════

	section .text

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ External Resources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

extern  free
extern  f668c4bd_resizeArray

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ b196167f_add ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  a8638224_add:function
a8638224_add:
; Parameters:
;	rdi : UnsignedIntArray *listArray
;	esi : uint32_t element
; Local Variables:
;	rax : listArray->values
;	edx : listArray->length
;	ecx : listArray->size

.prologue:                            ; functions typically have a prologue
	test       rdi, rdi               ; if (listArray == NULL)
	jz         .epilogue

	movdqa     xmm0, [rdi]            ; load all UnsignedIntArray properties into xmm0
	movhlps    xmm1, xmm0             ; move high xmm0 values into low xmm1
	movq       rax, xmm0              ; rax = listArray->values
	movd       edx, xmm1              ; edx = listArray->length
	psrldq     xmm1, 4                ; shift xmm1 right by four bytes
	movd       ecx, xmm1              ; ecx = listArray->size

.shouldWeResize:
	cmp        edx, ecx               ; if (listArray->length == listArray->size)
	je         .resizeArray

.addElement:
	mov        [rax + rdx*4], esi     ; listArray->values[listArray->length] = element
	inc        edx                    ; listArray->length++
	mov        [rdi+8], edx

.epilogue:                            ; functions typically have an epilogue
	ret                               ; pop return address from stack and jump there

.resizeArray:
	push       rdi                    ; save listArray
	push       rsi                    ; save element
	push       rdx                    ; save listArray->length

	; f668c4bd_resizeArray(listArray->values, listArray->length, sizeof(uint32_t), newLength)
	shl        ecx, 1                 ; newLength = (size * size)
	mov        [rdi+12], ecx          ; listArray->size = newLength

	mov        rdi, rax               ; arrayPtr = listArray->values
	mov        esi, edx               ; arrayLen = listArray->length
	mov        edx, SIZEOF_UINT       ; typeSize = sizeof(uint32_t)

	call       f668c4bd_resizeArray

	pop        rdx                    ; retrieve listArray->length
	pop        rsi                    ; retrieve element
	pop        rdi                    ; retrieve listArray

	mov        [rdi], rax             ; listArray->values = f668c4bd_resizeArray()
	jmp        .addElement

; ═════════════════════════════ Private Routines ═════════════════════════════
