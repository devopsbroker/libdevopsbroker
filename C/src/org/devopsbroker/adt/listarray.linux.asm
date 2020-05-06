;
; listarray.linux.asm - DevOpsBroker NASM file for the ListArray struct
;
; Copyright (C) 2019-2020 Edward Smith <edwardsmith@devopsbroker.org>
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
; Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-16
;
; This file defines the following x86-64 assembly language functions for the
; ListArray struct:
;
;   o void b196167f_add(ListArray *listArray, void *element);
;   o void b196167f_addAll(ListArray *listArray, void **elementArray, uint32_t numElements);
;   o void b196167f_addFromStack(ListArray *listArray, void *stack, uint32_t numElements);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════

; Constants
%define SIZEOF_PTR    0x08

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

	global  b196167f_add:function
b196167f_add:
; Parameters:
;	rdi : ListArray *listArray
;	rsi : void *element
; Local Variables:
;	edx : listArray->length
;	ecx : listArray->size
;	rcx : listArray->values

.prologue:                            ; functions typically have a prologue
	test       rdi, rdi               ; if (listArray == NULL)
	jz         .epilogue

	test       rsi, rsi               ; if (element == NULL)
	jz         .epilogue

.shouldWeResize:
	mov        edx, [rdi+8]           ; edx = listArray->length
	mov        ecx, [rdi+12]          ; ecx = listArray->size
	cmp        edx, ecx               ; if (listArray->length == listArray->size)
	jne        .addElement

.resizeArray:
	push       rdi                    ; save listArray
	push       rsi                    ; save element
	push       rdx                    ; save listArray->length

	; f668c4bd_resizeArray(listArray->values, listArray->length, sizeof(void*), newLength)
	shl        ecx, 1                 ; newLength = (size * size)
	mov        [rdi+12], ecx

	mov        rdi, [rdi]             ; listArray->values
	mov        esi, edx               ; listArray->length
	mov        edx, SIZEOF_PTR        ; sizeof(void*)

	call       f668c4bd_resizeArray

	pop        rdx                    ; retrieve listArray->length
	pop        rsi                    ; retrieve element
	pop        rdi                    ; retrieve listArray

	mov        [rdi], rax             ; listArray->values = f668c4bd_resizeArray()

.addElement:
	mov        rcx, [rdi]             ; set up rcx with listArray->values
	mov        [rcx + rdx*8], rsi     ; listArray->values[listArray->length] = element
	inc        edx                    ; listArray->length++
	mov        [rdi+8], edx

.epilogue:                            ; functions typically have an epilogue
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ b196167f_addAll ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  b196167f_addAll:function
b196167f_addAll:
; Parameters:
;	rdi : ListArray *listArray
;	rsi : void **elementArray
;	rdx : uint32_t numElements
; Local Variables:

.prologue:                            ; functions typically have a prologue
	test       rsi, rsi               ; if (elementArray == NULL)
	jz         .epilogue

	test       rdx, rdx               ; if (numElements == 0)
	jz         .epilogue

.shouldWeResize:
	mov        ecx, edx               ; ecx = numElements
	add        edx, [rdi+8]           ; newLength = listArray->length + numElements
	cmp        edx, [rdi+12]          ; if (newLength > listArray->size)
	jbe        .addElements

.resizeArray:
	push       rdi                    ; save listArray
	push       rsi                    ; save elementArray
	push       rdx                    ; save newLength
	push       rcx                    ; save numElements
	sub        rsp, 8                 ; align stack frame before calling f668c4bd_resizeArray()

	; f668c4bd_resizeArray(listArray->values, listArray->length, sizeof(void*), newSize)
	mov        ecx, edx               ; newSize = (newLength * 1.5)
	shr        edx, 1
	add        ecx, edx
	mov        [rdi+12], ecx

	mov        edx, SIZEOF_PTR
	mov        esi, [rdi+8]
	mov        rdi, [rdi]

	call       f668c4bd_resizeArray

	add        rsp, 8                 ; re-align stack frame after function call
	pop        rcx                    ; retrieve numElements
	pop        rdx                    ; retrieve newLength
	pop        rsi                    ; retrieve stack
	pop        rdi                    ; retrieve listArray

	mov        [rdi], rax             ; listArray->values = f668c4bd_resizeArray()

.addElements:
	xchg       edx, [rdi+8]           ; swap newLength with listArray->length
	mov        rdi, [rdi]             ; set up rdi with listArray->values for movsq
	shl        edx, 3                 ; rdi += (listArray->length *= 8)
	add        rdi, rdx

	rep movsq                         ; move ecx quadwords from void **elementArray to listArray->values

.epilogue:                            ; functions typically have an epilogue
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~ b196167f_addFromStack ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  b196167f_addFromStack:function
b196167f_addFromStack:
; Parameters:
;	rdi : ListArray *listArray
;	rsi : void **stack
;	rdx : uint32_t numElements
; Local Variables:
;	rdi : listArray->values destination
;	rsi : void **elementArray source
;	rdx : listArray->values
;	ecx : original uint32_t numElements value
;	r8d : listArray->size
;	r9d : newLength

.prologue:                            ; functions typically have a prologue
	test       rsi, rsi               ; if (elementArray == NULL)
	jz         .epilogue

	test       rdx, rdx               ; if (numElements == 0)
	jz         .epilogue

.shouldWeResize:
	mov        ecx, edx               ; ecx = numElements
	add        edx, [rdi+8]           ; newLength = listArray->length + numElements
	cmp        edx, [rdi+12]          ; if (newLength > listArray->size)
	jbe        .addElements

.resizeArray:
	push       rdi                    ; save listArray
	push       rsi                    ; save stack
	push       rdx                    ; save newLength
	push       rcx                    ; save numElements
	sub        rsp, 8                 ; align stack frame before calling f668c4bd_resizeArray()

	; f668c4bd_resizeArray(listArray->values, listArray->length, sizeof(void*), newSize)
	mov        ecx, edx               ; newSize = (newLength * 1.5)
	shr        edx, 1
	add        ecx, edx
	mov        [rdi+12], ecx

	mov        edx, SIZEOF_PTR
	mov        esi, [rdi+8]
	mov        rdi, [rdi]

	call       f668c4bd_resizeArray

	add        rsp, 8                 ; re-align stack frame after function call
	pop        rcx                    ; retrieve numElements
	pop        rdx                    ; retrieve newLength
	pop        rsi                    ; retrieve stack
	pop        rdi                    ; retrieve listArray

	mov        [rdi], rax             ; listArray->values = f668c4bd_resizeArray()

.addElements:
	xchg       edx, [rdi+8]           ; swap newLength with listArray->length
	mov        rdi, [rdi]             ; set up rdi with listArray->values
	shl        edx, 3                 ; rdi += (listArray->length *= 8)
	add        rdi, rdx

.whileElements:
	dec        ecx                    ; numElements--
	mov        rax, [rsi]             ; listArray->values[i] = stack[j]
	mov        [rdi], rax
	sub        rsi, SIZEOF_PTR        ; j--
	add        rdi, SIZEOF_PTR        ; i++

	test       ecx, ecx               ; if (numElements > 0)
	jnz        .whileElements

.epilogue:                            ; functions typically have an epilogue
	ret                               ; pop return address from stack and jump there

; ═════════════════════════════ Private Routines ═════════════════════════════
