;
; memory.linux.asm - DevOpsBroker NASM file for memory-related functionality
;
; Copyright (C) 2018-2019 Edward Smith <edwardsmith@devopsbroker.org>
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
; Developed on Ubuntu 18.04.1 LTS running kernel.osrelease = 4.15.0-42
;
; This file implements the following x86-64 assembly language functions for the
; org.devopsbroker.lang.memory.h header file:
;
;   o void *f668c4bd_malloc(size_t size);
;   o void *f668c4bd_mallocArray(size_t typeSize, size_t numBlocks);
;   o void *f668c4bd_mallocFlexibleStruct(size_t structSize, size_t typeSize, size_t numBlocks);
;   o void f668c4bd_memcopy(void *source, void *dest, size_t numBytes);
;   o void f668c4bd_meminit(void *ptr, size_t size);
;   o void *f668c4bd_realloc(void *ptr, size_t origSize, size_t newSize);
;   o void *f668c4bd_resizeArray(void *arrayPtr, uint32_t arrayLen, size_t typeSize, size_t numBlocks);
;   o void *f668c4bd_stralloc(size_t size);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════


; ═════════════════════════════ Initialized Data ═════════════════════════════

section .data               ; DX directives

; ════════════════════════════ Uninitialized Data ════════════════════════════

section .bss                ; RESX directives

; ══════════════════════════════ Assembly Code ═══════════════════════════════

	section .text

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ External Resources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

extern  aligned_alloc
extern  malloc
extern  free
extern  abort

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~ f668c4bd_alignedAlloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f668c4bd_alignedAlloc:function
f668c4bd_alignedAlloc:
; Parameters:
;	rdi : size_t alignment
;	rsi : size_t size
; Local Variables:

.prologue:                            ; functions typically have a prologue
	sub        rsp, 8                 ; align stack frame before calling aligned_alloc()

.aligned_alloc:
	call       aligned_alloc WRT ..plt

	test       rax, rax               ; if (ptr == NULL)
	jz         .fatalError

.epilogue:                            ; functions typically have an epilogue
	add        rsp, 8                 ; re-align stack frame before return
	ret                               ; pop return address from stack and jump there

.fatalError:
	call       abort WRT ..plt

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f668c4bd_malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f668c4bd_malloc:function
f668c4bd_malloc:
; Parameters:
;	rdi : size_t size / alignment parameter for aligned_alloc
; Local Variables:
;	rsi : size parameter for aligned_alloc

.prologue:                            ; functions typically have a prologue
	sub        rsp, 8                 ; align stack frame before calling aligned_alloc()
	mov        rsi, rdi               ; set size parameter for aligned_alloc
	mov        edi, 0x0F              ; set alignment parameter for aligned_alloc

.calcAllocSize:                       ; size must be a multiple of 16-byte alignment
	add        rsi, rdi               ; size = ((size + 15) / 16) * 16
	jo         .fatalError            ; fatal error if overflow
	shr        rsi, 4
	shl        rsi, 4

.aligned_alloc:
	inc        dil                    ; alignment++
	call       aligned_alloc WRT ..plt

	test       rax, rax               ; if (ptr == NULL)
	je         .fatalError

.epilogue:                            ; functions typically have an epilogue
	add        rsp, 8                 ; re-align stack frame before return
	ret                               ; pop return address from stack and jump there

.fatalError:
	call       abort WRT ..plt

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~ f668c4bd_mallocArray ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f668c4bd_mallocArray:function
f668c4bd_mallocArray:
; Parameters:
;	rdi : size_t typeSize / alignment parameter for aligned_alloc
;	rsi : size_t numBlocks
; Local Variables:
;	rax : both source and low-quadword destination for mul
;	rdx : high-quadword destination for mul
;	rsi : size parameter for aligned_alloc

.prologue:                            ; functions typically have a prologue
	sub        rsp, 8                 ; align stack frame before calling aligned_alloc()
	mov        rax, rdi               ; set mul source to typeSize
	mov        edi, 0x0F              ; set alignment parameter for aligned_alloc

.calcAllocSize:                       ; size must be a multiple of 16-byte alignment
	mul        rsi                    ; rax *= rsi
	jo         .fatalError            ; fatal error if overflow

	mov        rsi, rax               ; set size parameter for aligned_alloc
	add        rsi, rdi               ; size = ((size + 15) / 16) * 16
	jo         .fatalError            ; fatal error if overflow
	shr        rsi, 4
	shl        rsi, 4

.aligned_alloc:
	inc        dil                    ; alignment++
	call       aligned_alloc WRT ..plt

	test       rax, rax               ; if (ptr == NULL)
	jz         .fatalError

.epilogue:                            ; functions typically have an epilogue
	add        rsp, 8                 ; re-align stack frame before return
	ret                               ; pop return address from stack and jump there

.fatalError:
	call       abort WRT ..plt

; ~~~~~~~~~~~~~~~~~~~~~~~ f668c4bd_mallocFlexibleStruct ~~~~~~~~~~~~~~~~~~~~~~~

	global  f668c4bd_mallocFlexibleStruct:function
f668c4bd_mallocFlexibleStruct:
; Parameters:
;	rdi : size_t structSize
;	      alignment parameter for aligned_alloc
;	rsi : size_t typeSize
;	      size parameter for aligned_alloc
;	rdx : size_t numBlocks
;	      high-quadword destination for mul
; Local Variables:
;	rax : source 1 for mul operation
;	      low-quadword destination for mul

.prologue:                            ; functions typically have a prologue
	sub        rsp, 8                 ; align stack frame before calling aligned_alloc()

.calcAllocSize:                       ; size must be a multiple of 16-byte alignment
	mov        rax, rsi               ; set mul source 1 to typeSize

	mul        rdx                    ; allocSize = (typeSize * numBlocks)
	jo         .fatalError            ; fatal error if overflow

	add        rax, rdi               ; allocSize += structSize
	jo         .fatalError            ; fatal error if overflow

	mov        edi, 0x0F              ; size = ((size + 15) / 16) * 16
	add        rax, rdi
	jo         .fatalError            ; fatal error if overflow
	shr        rax, 4
	shl        rax, 4

.aligned_alloc:
	inc        dil                    ; alignment = 16
	mov        rsi, rax               ; rsi = 16-byte aligned size
	call       aligned_alloc WRT ..plt

	test       rax, rax               ; if (ptr == NULL)
	jz         .fatalError

.epilogue:                            ; functions typically have an epilogue
	add        rsp, 8                 ; re-align stack frame before return
	ret                               ; pop return address from stack and jump there

.fatalError:
	call       abort WRT ..plt

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f668c4bd_memcopy ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f668c4bd_memcopy:function
f668c4bd_memcopy:

.prologue:                            ; functions typically have a prologue
	cmp        rdx, 0x08
	jae        .eightBytes
	cmp        dl, 0x04
	jae        .fourBytes
	jnz        .bytes
	jz         .epilogue

.eightBytes:
	mov        rcx, [rdi]
	mov        [rsi], rcx

	sub        rdx, 0x08
	jz         .epilogue

	add        rdi, 0x08
	add        rsi, 0x08

	cmp        rdx, 0x08
	jae        .eightBytes
	cmp        dl, 0x04
	jb         .bytes

.fourBytes:
	mov        ecx, [rdi]
	mov        [rsi], ecx

	sub        sil, 0x04
	jz         .epilogue

	add        rdi, 0x04
	add        rsi, 0x04

.bytes:
	mov        cl, [rdi]
	mov        [rsi], cl

	inc        rdi
	inc        rsi
	dec        cl

	jnz        .bytes

.epilogue:                            ; functions typically have an epilogue
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f668c4bd_meminit ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f668c4bd_meminit:function
f668c4bd_meminit:
; Parameters:
;	rdi : void *ptr
;	rsi : size_t size
;	    : num bytes to copy
; Local Variables:
;	rax : number of 64-bit blocks to initialize
;	rcx : loop counter
;	rdx : temporary variable
;	r8  : zero value register

.prologue:                            ; functions typically have a prologue
	test       rdi, rdi               ; if (ptr == NULL)
	jz         .epilogue

	test       rsi, rsi               ; if (size == 0)
	jz         .epilogue

	xor        rcx, rcx               ; set loop counter to zero
	xor        r8, r8                 ; zero out r8 for memory initialization

.calcNumBlocksToInit:
	mov        rax, rsi               ; numBlocks = numBytes / 8
	shr        rax, 3
	jz         .initBytes             ; if (numBlocks == 0)

	mov        rdx, rax               ; numBytes -= (numBlocks * 8)
	shl        rdx, 3
	sub        rsi, rdx

.initBlocks:
	mov        [rdi + 8*rcx], r8      ; copy zero into ptr address

	inc        rcx                    ; loopCounter++
	dec        rax                    ; numBlocks--
	jnz        .initBlocks

	test       rsi, rsi               ; if (numBytes == 0)
	jz         .epilogue

	shl        rcx, 3                 ; ptr += (numBlocks * 8)
	add        rdi, rcx

.initBytes:
	mov        [rdi], r8b             ; copy zero into ptr address
	inc        rdi                    ; ptr++
	dec        sil                    ; numBytes--
	jnz        .initBytes             ; if (numBytes != 0)

.epilogue:                            ; functions typically have an epilogue
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f668c4bd_realloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f668c4bd_realloc:function
f668c4bd_realloc:
; Parameters:
;	rdi : void *ptr
;	rsi : size_t origSize
;	rdx : size_t newSize
; Local Variables:
;	rax : void *newPtr

.prologue:                            ; functions typically have a prologue
	mov        eax, 0x0F              ; rax = 15

.align:
	add        rsi, rax               ; origSize = ((origSize + 15) / 16) * 16
	add        rdx, rax               ; newSize = ((newSize + 15) / 16) * 16

	shr        rsi, 4
	shr        rdx, 4

	shl        rsi, 4
	shl        rdx, 4

.malloc:
	cmp        rsi, rdx               ; if (origSize > newSize)
	ja         .fatalError

	push       rdi                    ; save void *ptr
	push       rsi                    ; save size_t origSize
	sub        rsp, 8                 ; align stack frame before calling malloc()

	mov        rdi, rax               ; set size parameter for malloc() call
	call       malloc WRT ..plt

	test       rax, rax               ; if (ptr == NULL)
	je         .fatalError

	add        rsp, 8
	pop        rsi                    ; retrieve size_t origSize
	pop        rdi                    ; retrieve void *ptr
	push       rax                    ; save void *newPtr

.copy:
	prefetcht0 [rdi]                  ; prefetch the pointer data into the CPU cache
	mov        rcx, rdi               ; save void *ptr
	shl        rsi, 4                 ; rsi = numCopies
	mov        edx, 0x10              ; rdx = 16

.whileData:
	movdqa     xmm0, [rdi]            ; newPtr[data] = ptr[data]
	movdqa     [rax], xmm0

	dec        rsi                    ; numCopies--
	test       rsi, rsi               ; if (numCopies == 0)
	jz         .free

	add        rdi, rdx               ; ptr += 16
	add        rax, rdx               ; newPtr += 16
	jmp        .whileData

.free:
	mov        rdi, rcx               ; set ptr parameter for free() call
	call       free WRT ..plt

.epilogue:                            ; functions typically have an epilogue
	pop        rax                    ; retrieve void *newPtr
	ret                               ; pop return address from stack and jump there

.fatalError:
	call       abort WRT ..plt

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~ f668c4bd_resizeArray ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f668c4bd_resizeArray:function
f668c4bd_resizeArray:
; Parameters:
;	rdi : void *arrayPtr
;	    : alignment parameter for aligned_alloc
;	esi : uint32_t arrayLen
;	    : size parameter for aligned_alloc
;	rdx : size_t typeSize
;	rcx : size_t numBlocks
;	    : loop counter
; Local Variables:
;	rax : both source and low-quadword destination for mul
;	    : num bytes to copy
;	rdx : high-quadword destination for mul
;	r8  : 64-bit data buffer

.prologue:                            ; functions typically have a prologue
	push       r12                    ; preserve r12 register
	push       r13                    ; preserve r13 register
	push       r14                    ; preserve r14 register

	test       rdi, rdi               ; if (arrayPtr == NULL)
	jz         .fatalError

	test       esi, esi               ; if (arrayLen == 0)
	jz         .fatalError

	test       rdx, rdx               ; if (typeSize == 0)
	jz         .fatalError

	test       rcx, rcx               ; if (numBlocks == 0)
	jz         .fatalError

	mov        rax, rdx               ; set mul source to typeSize
	mov        r12, rdi               ; save arrayPtr for later
	mov        r13, rsi               ; save arrayLen for later
	mov        r14, rdx               ; save typeSize for later
	mov        edi, 0x0F              ; set alignment parameter for aligned_alloc

.calcAllocSize:                       ; size must be a multiple of 16-byte alignment
	mul        rcx                    ; rax *= rcx
	jo         .fatalError            ; fatal error if overflow

	mov        rsi, rax               ; set size parameter for aligned_alloc
	add        rsi, rdi               ; size = ((size + 15) / 16) * 16
	jo         .fatalError            ; fatal error if overflow
	shr        rsi, 4
	shl        rsi, 4

.aligned_alloc:
	inc        dil                    ; alignment++
	call       aligned_alloc WRT ..plt

	test       rax, rax               ; if (ptr == NULL)
	je         .fatalError

.calcNumBytesToCopy:
	prefetcht0 [r12]                  ; prefetch array values into the CPU cache
	mov        rdi, rax               ; save new arrayPtr into rdi
	xor        rcx, rcx               ; set loop counter to zero

	mov        rax, r13               ; set mul source to arrayLen
	mul        r14                    ; rax *= r14
	jo         .fatalError            ; fatal error if overflow

	add        rax, 0x07              ; numBlocks = (numBytes + 7) / 8
	jo         .fatalError            ; fatal error if overflow
	shr        rax, 3

.copyData:
	mov        r8, [r12 + 8*rcx]      ; copy qword from arrayPtr address into r8
	mov        [rdi + 8*rcx], r8      ; copy r8 into new arrayPtr address

	inc        rcx                    ; loopCounter++
	dec        rax                    ; numBlocks--
	jnz        .copyData

.freeOriginalArray:
	xchg       r12, rdi               ; swap new arrayPtr with old arrayPtr
	call       free WRT ..plt

.epilogue:                            ; functions typically have an epilogue
	mov        rax, r12               ; return aligned_alloc() value
	pop        r14                    ; restore r14 register
	pop        r13                    ; restore r13 register
	pop        r12                    ; restore r12 register
	ret                               ; pop return address from stack and jump there

.fatalError:
	call       abort WRT ..plt

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f668c4bd_stralloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f668c4bd_stralloc:function
f668c4bd_stralloc:
; Parameters:
;	rdi : size_t size
;	    : size_t alignment parameter for aligned_alloc
; Local Variables:
;	rsi : size_t size parameter for aligned_alloc
; Notes:
;   I always want an extra 16-byte block when the value is a multiple of 16
;   which is why I calculate the aligned_alloc size parameter the way I do

.prologue:                            ; functions typically have a prologue
	sub        rsp, 8                 ; align stack frame before calling aligned_alloc()
	mov        rsi, rdi               ; set size parameter for aligned_alloc
	mov        edi, 0x10              ; set alignment parameter for aligned_alloc

.calcAllocSize:                       ; size must be a multiple of 16-byte alignment
	shr        rsi, 4                 ; size = ((size / 16) + 1) * 16
	inc        rsi
	shl        rsi, 4
	jz         .fatalError            ; fatal error if overflow

.aligned_alloc:
	call       aligned_alloc WRT ..plt

	test       rax, rax               ; if (ptr == NULL)
	jz         .fatalError

.epilogue:                            ; functions typically have an epilogue
	add        rsp, 8                 ; re-align stack frame before return
	ret                               ; pop return address from stack and jump there

.fatalError:
	call       abort WRT ..plt

; ═════════════════════════════ Private Routines ═════════════════════════════
