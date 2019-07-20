;
; listarray.linux.asm - DevOpsBroker NASM file for the ListArray struct
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
; Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-16
;
; This file defines the following x86-64 assembly language functions for the
; ListArray struct:
;
;   o void b196167f_add(ListArray *listArray, void *element);
;   o void b196167f_addAll(ListArray *listArray, void **elementArray, uint32_t numElements);
;   o void b196167f_destroyAllElements(ListArray *listArray);
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

extern  reallocarray
extern  malloc
extern  free
extern  abort
extern  f668c4bd_resizeArray

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ b196167f_add ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  b196167f_add:function
b196167f_add:
; Parameters:
;	rdi : ListArray *listArray
;	rsi : void *element
; Local Variables:
;	rdx : listArray->values
;	ecx : listArray->length
;	r8d : listArray->size

.prologue:                            ; functions typically have a prologue
	mov        rdx, [rdi]             ; load listArray->values into rdx
	mov        ecx, [rdi+8]           ; load listArray->length into ecx
	mov        r8d, [rdi+12]          ; load listArray->size into r8d

.shouldWeResize:
	cmp        ecx, r8d               ; if (listArray->length <= listArray->size)
	jbe        .addElement

.resize:
	shl        r8d, 1                 ; listArray->size *= 2
	mov        [rdi+12], r8d

	call       resizeArray

.addElement:
	mov        [rdx + rcx*8], rsi     ; save element into values[length]
	inc        ecx                    ; listArray->length++
	mov        [rdi+8], ecx

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

	mov        ecx, edx               ; move numElements to ecx
	mov        r9d, edx               ; move numElements to r9d
	mov        rdx, [rdi]             ; load listArray->values into rdx
	add        r9d, [rdi+8]           ; newLength = listArray->length + numElements
	mov        r8d, [rdi+12]          ; load listArray->size into r8d

.shouldWeResize:
	mov        [rdi+8], r9d           ; listArray->length = newLength
	cmp        r9d, r8d               ; if (newLength <= listArray->size)
	jbe        .addElements

.resize:
	mov        r8d, r9d               ; listArray->size = (newLength + newLength / 2)
	shr        r8d, 1
	add        r8d, r9d
	mov        [rdi+12], r8d

	call       resizeArray

.addElements:
	mov        rdi, rdx               ; rdi = listArray->values
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
	mov        esi, [rdi+8]           ; f668c4bd_resizeArray(listArray->values, listArray->length, sizeof(void*), newSize)
	mov        ecx, edx               ; newSize = (newLength * 1.5)
	shr        edx, 1
	add        ecx, edx
	mov        [rdi+12], ecx
	mov        edx, SIZEOF_PTR

	call       f668c4bd_resizeArray

	add        rsp, 8                 ; re-align stack frame after function call
	pop        rcx                    ; retrieve numElements
	pop        rdx                    ; retrieve newLength
	pop        rsi                    ; retrieve stack
	pop        rdi                    ; retrieve listArray

	mov        [rdi], rax             ; listArray->values = f668c4bd_resizeArray()

.addElements:
	mov        rdi, [rdi]             ; set up rdi with listArray->values for movsb
	xchg       edx, [rdi+8]           ; swap newLength with listArray->length
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

; ~~~~~~~~~~~~~~~~~~~~~~~ b196167f_destroyAllElements ~~~~~~~~~~~~~~~~~~~~~~~~

	global  b196167f_destroyAllElements:function
b196167f_destroyAllElements:
; Parameters:
;	rdi : ListArray *listArray
; Local Variables:
;	rsi : listArray->values
;	rdx : listArray->length

.prologue:                            ; functions typically have a prologue
	mov        rsi, [rdi]             ; load listArray->values into rsi
	mov        edx, [rdi+8]           ; load listArray->length into edx

	test       edx, edx               ; if (listArray->length == 0)
	jz         .emptyListArray

	sub        rsp, 8                 ; align stack frame before calling free()

.whileData:
	lodsq                             ; load quadword from listArray->values
	dec        edx                    ; listArray->length--

.free:                                ; free(listArray->values[i])
	push       rsi                    ; save listArray->values
	push       rdx                    ; save listArray->length

	mov        rdi, rax
	call       free WRT ..plt

	pop        rdx                    ; retrieve listArray->length
	pop        rsi                    ; retrieve listArray->values

	test       edx, edx               ; if (listArray->length != 0)
	jnz        .whileData

.epilogue:
	add        rsp, 8                 ; unalign stack frame after calling free()
	mov        [rdi+8], edx           ; save listArray->length
	ret                               ; pop return address from stack and jump there

.emptyListArray:
	ret                               ; pop return address from stack and jump there

; ═════════════════════════════ Private Routines ═════════════════════════════

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ resizeArray ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

resizeArray:
; Parameters:
;	rdi : ListArray *listArray
;	rsi : void *element
; Local Variables:
;	rdx : listArray->values
;	ecx : listArray->length
;	r8d : listArray->size

	push       rsi                    ; save void *element
	push       rcx                    ; save listArray->length
	push       rdi                    ; save ListArray *listArray
	sub        rsp, 8                 ; align stack frame before calling reallocarray()

	mov        rdi, rdx               ; reallocarray(values, size, sizeof(void*))
	mov        esi, r8d
	mov        edx, 0x08
	call       reallocarray WRT ..plt

	test       rax, rax               ; if (ptr == NULL)
	jz         .fatalError

	add        rsp, 8                 ; unalign stack frame after calling reallocarray()
	pop        rdi                    ; retrieve ListArray *listArray
	pop        rcx                    ; retrieve listArray->length
	pop        rsi                    ; retrieve void *element

.epilogue:
	mov        [rdi], rax             ; listArray->values = reallocarray()
	mov        rdx, rax               ; rdx = newPtr
	ret                               ; pop return address from stack and jump there

.fatalError:
	call       abort WRT ..plt
