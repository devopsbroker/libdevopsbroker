;
; md5.linux.asm - DevOpsBroker NASM file for MD5 hash functionality
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
; Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-59
;
; This file implements the following x86-64 assembly language functions for the
; org.devopsbroker.hash.md5.h header file:
;
;   o char *f1518caf_getHexString(uint32_t *state);
;   o void f1518caf_md5Transform(uint32_t *state, void *block);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════

; character values
%define ZERO        0x30
%define UPPER_W     0x57

%define MD5_HASH_LENGTH  32

; ═════════════════════════════ Initialized Data ═════════════════════════════

section .data               ; DX directives
 align 4

; Precomputed table for the binary integer part of the sines of integers (Radians)
X:          dd 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,\
               0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,\
			   0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,\
			   0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,\
			   0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,\
			   0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,\
			   0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,\
			   0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,\
			   0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,\
			   0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,\
			   0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,\
			   0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,\
			   0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,\
			   0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,\
			   0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,\
			   0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391

; ════════════════════════════ Uninitialized Data ════════════════════════════

section .bss                ; RESX directives

buffer:     resb 64         ; reserve 64 bytes

; ══════════════════════════════ Assembly Code ═══════════════════════════════

section .text               ; TEXT section

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ External Resources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

extern  f668c4bd_stralloc

; ~~~~~~~~~~~~~~~~~~~~~~~~~~ f1518caf_getHexString ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f1518caf_getHexString:function
f1518caf_getHexString:
; Parameters:
;	rdi : uint32_t *state

.prologue:                            ; functions typically have a prologue
	push       rdi                    ; put rdi onto the stack

	; void *f668c4bd_stralloc(size_t size);
.allocateMD5String:
	mov        rdi, MD5_HASH_LENGTH
	call       f668c4bd_stralloc
	mov        rdi, rax
	pop        rsi                    ; retrieve rdi from the stack and place in rsi

	mov        rdx, [rsi]
	mov        r8w, ZERO
	mov        r9w, UPPER_W
	xor        ecx, ecx
	xor        r10d, r10d

.convertToHexadecimal:
	movzx      r11d, dl               ; baseChar = (value <= 9) ? ZERO : UPPER_W
	and        r11b, 0xF0
	shr        r11b, 4
	cmp        r11b, 0x09
	cmovbe     cx, r8w
	cmova      cx, r9w

	lea        r11d, [r11d + ecx]     ; char = baseChar + value
	mov        [rdi], r11b
	inc        rdi                    ; md5String++

	movzx      r11d, dl               ; baseChar = (value <= 9) ? ZERO : UPPER_W
	and        r11b, 0x0F
	cmp        r11b, 0x09
	cmovbe     cx, r8w
	cmova      cx, r9w

	lea        r11d, [r11d + ecx]     ; char = baseChar + value
	mov        [rdi], r11b
	inc        rdi                    ; md5String++

	shr        rdx, 8                 ; move one full byte over
	inc        r10b                   ; loop counter++

	test       r10b, 0x07             ; if (loop counter % 8 != 0)
	jnz        .convertToHexadecimal

	cmp        r10b, 0x10             ; if (loop counter == 16)
	je         .epilogue

	mov        rdx, [rsi + r10]
	jmp        .convertToHexadecimal

.epilogue:                            ; functions typically have an epilogue
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~ f1518caf_md5Transform ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f1518caf_md5Transform:function
f1518caf_md5Transform:
; Parameters:
;	rdi : uint32_t *state
;	rsi : void *block
; Local Variables:
;	eax : state[0]
;	ebx : state[1]
;	ecx : state[2]
;	edx : state[3]
;	r8  : loop index
;	r9  : rotate left by count
;	r10 : return address register

.prologue:                            ; functions typically have a prologue
	push       rbx                    ; put rbx onto the stack
	push       r12                    ; put r12 onto the stack
	push       r13                    ; put r13 onto the stack
	sub        rsp, 16                ; make room for uint32_t a, b, c, d

	mov        eax, [rdi]             ; a = state[0]
	mov        ebx, [rdi + 4]         ; b = state[1]
	mov        ecx, [rdi + 8]         ; c = state[2]
	mov        edx, [rdi + 12]        ; d = state[3]
	xor        r8d, r8d               ; i = 0

	mov        [rsp], eax             ; save a
	mov        [rsp + 4], ebx         ; save b
	mov        [rsp + 8], ecx         ; save c
	mov        [rsp + 12], edx        ; save d

.performFFTransform:
	mov        r9b, 0x07              ; s = 7

	lea        r10, [rel $+12]
	jmp        ffTransform

	inc        r8b                    ; loop index++
	mov        [rsp], eax             ; a = ffTransform()
	mov        eax, [rsp + 12]        ; set up for next ffTransform
	mov        ebx, [rsp]
	mov        ecx, [rsp + 4]
	mov        edx, [rsp + 8]
	mov        r9b, 0x0C              ; s = 12

	lea        r10, [rel $+12]
	jmp        ffTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 12], eax        ; d = ffTransform()
	mov        eax, [rsp + 8]         ; set up for next ffTransform
	mov        ebx, [rsp + 12]
	mov        ecx, [rsp]
	mov        edx, [rsp + 4]
	mov        r9b, 0x11              ; s = 17

	lea        r10, [rel $+12]
	jmp        ffTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 8], eax         ; c = ffTransform()
	mov        eax, [rsp + 4]         ; set up for next ffTransform
	mov        ebx, [rsp + 8]
	mov        ecx, [rsp + 12]
	mov        edx, [rsp]
	mov        r9b, 0x16              ; s = 22

	lea        r10, [rel $+12]
	jmp        ffTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 4], eax         ; b = ffTransform()
	mov        eax, [rsp]             ; set up for next ffTransform
	mov        ebx, [rsp + 4]
	mov        ecx, [rsp + 8]
	mov        edx, [rsp + 12]
	cmp        r8b, 0x10              ; if (loop index < 16)
	jb         .performFFTransform

.performGGTransform:
	mov        r9b, 0x05              ; s = 5

	lea        r10, [rel $+12]
	jmp        ggTransform

	inc        r8b                    ; loop index++
	mov        [rsp], eax             ; a = ggTransform()
	mov        eax, [rsp + 12]        ; set up for next ggTransform
	mov        ebx, [rsp]
	mov        ecx, [rsp + 4]
	mov        edx, [rsp + 8]
	mov        r9b, 0x09              ; s = 9

	lea        r10, [rel $+12]
	jmp        ggTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 12], eax        ; d = ggTransform()
	mov        eax, [rsp + 8]         ; set up for next ggTransform
	mov        ebx, [rsp + 12]
	mov        ecx, [rsp]
	mov        edx, [rsp + 4]
	mov        r9b, 0x0E              ; s = 14

	lea        r10, [rel $+12]
	jmp        ggTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 8], eax         ; c = ggTransform()
	mov        eax, [rsp + 4]         ; set up for next ggTransform
	mov        ebx, [rsp + 8]
	mov        ecx, [rsp + 12]
	mov        edx, [rsp]
	mov        r9b, 0x14              ; s = 20

	lea        r10, [rel $+12]
	jmp        ggTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 4], eax         ; b = ggTransform()
	mov        eax, [rsp]             ; set up for next ggTransform
	mov        ebx, [rsp + 4]
	mov        ecx, [rsp + 8]
	mov        edx, [rsp + 12]
	cmp        r8b, 0x20              ; if (loop index < 32)
	jb         .performGGTransform

.performHHTransform:
	mov        r9b, 0x04              ; s = 4

	lea        r10, [rel $+12]
	jmp        hhTransform

	inc        r8b                    ; loop index++
	mov        [rsp], eax             ; a = hhTransform()
	mov        eax, [rsp + 12]        ; set up for next hhTransform
	mov        ebx, [rsp]
	mov        ecx, [rsp + 4]
	mov        edx, [rsp + 8]
	mov        r9b, 0x0B              ; s = 11

	lea        r10, [rel $+12]
	jmp        hhTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 12], eax        ; d = hhTransform()
	mov        eax, [rsp + 8]         ; set up for next hhTransform
	mov        ebx, [rsp + 12]
	mov        ecx, [rsp]
	mov        edx, [rsp + 4]
	mov        r9b, 0x10              ; s = 16

	lea        r10, [rel $+12]
	jmp        hhTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 8], eax         ; c = hhTransform()
	mov        eax, [rsp + 4]         ; set up for next hhTransform
	mov        ebx, [rsp + 8]
	mov        ecx, [rsp + 12]
	mov        edx, [rsp]
	mov        r9b, 0x17              ; s = 23

	lea        r10, [rel $+12]
	jmp        hhTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 4], eax         ; b = hhTransform()
	mov        eax, [rsp]             ; set up for next hhTransform
	mov        ebx, [rsp + 4]
	mov        ecx, [rsp + 8]
	mov        edx, [rsp + 12]
	cmp        r8b, 0x30              ; if (loop index < 48)
	jb         .performHHTransform

.performIITransform:
	mov        r9b, 0x06              ; s = 6

	lea        r10, [rel $+12]
	jmp        iiTransform

	inc        r8b                    ; loop index++
	mov        [rsp], eax             ; a = iiTransform()
	mov        eax, [rsp + 12]        ; set up for next iiTransform
	mov        ebx, [rsp]
	mov        ecx, [rsp + 4]
	mov        edx, [rsp + 8]
	mov        r9b, 0x0A              ; s = 10

	lea        r10, [rel $+12]
	jmp        iiTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 12], eax        ; d = iiTransform()
	mov        eax, [rsp + 8]         ; set up for next iiTransform
	mov        ebx, [rsp + 12]
	mov        ecx, [rsp]
	mov        edx, [rsp + 4]
	mov        r9b, 0x0F              ; s = 15

	lea        r10, [rel $+12]
	jmp        iiTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 8], eax         ; c = iiTransform()
	mov        eax, [rsp + 4]         ; set up for next iiTransform
	mov        ebx, [rsp + 8]
	mov        ecx, [rsp + 12]
	mov        edx, [rsp]
	mov        r9b, 0x15              ; s = 21

	lea        r10, [rel $+12]
	jmp        iiTransform

	inc        r8b                    ; loop index++
	mov        [rsp + 4], eax         ; b = iiTransform()
	mov        eax, [rsp]             ; set up for next iiTransform
	mov        ebx, [rsp + 4]
	mov        ecx, [rsp + 8]
	mov        edx, [rsp + 12]
	cmp        r8b, 0x40              ; if (loop index < 64)
	jb         .performIITransform

.addChunkHashResult:
	add        [rdi], eax             ; state[0] += a
	add        [rdi + 4], ebx         ; state[1] += b
	add        [rdi + 8], ecx         ; state[2] += c
	add        [rdi + 12], edx        ; state[3] += d

.epilogue:                            ; functions typically have an epilogue
	add        rsp, 16                ; reset stack pointer before popping registers
	pop        r13                    ; retrieve r13 from the stack
	pop        r12                    ; retrieve r12 from the stack
	pop        rbx                    ; retrieve rbx from the stack
	ret                               ; pop return address from stack and jump there

; ═════════════════════════════ Private Routines ═════════════════════════════

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ffTransform ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ffTransform:
	mov        r11d, ebx              ; F := (B and C) or ((not B) and D)
	and        r11d, ecx
	mov        r12d, ebx
	not        r12d
	and        r12d, edx
	or         r11d, r12d

	lea        r12, [rel X]           ; A += F (b,c,d) + X + (UINT4)(block[i]);
	add        r11d, [r12 + 4*r8]
	add        r11d, dword [rsi + 4*r8]
	add        eax, r11d

	mov        r12b, cl
	mov        cl, r9b
	rol        eax, cl                ; A := leftrotate(A, s[i]);
	add        eax, ebx               ; A += B
	mov        cl, r12b

.epilogue:                            ; functions typically have an epilogue
	jmp        r10                    ; jump to the return address

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ggTransform ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ggTransform:
	mov        r11d, ebx              ; G := (B and D) or (C and (not D))
	and        r11d, edx
	mov        r12d, edx
	not        r12d
	and        r12d, ecx
	or         r11d, r12d

	lea        r13d, [5*r8d + 1]      ; g := (5×i + 1) mod 16
	and        r13d, 0x0F

	lea        r12, [rel X]           ; A += G (b,c,d) + X + (UINT4)(block[g]);
	add        r11d, [r12 + 4*r8]
	add        r11d, dword [rsi + 4*r13]
	add        eax, r11d

	mov        r12b, cl
	mov        cl, r9b
	rol        eax, cl                ; A := leftrotate(A, s[i]);
	add        eax, ebx               ; A += B
	mov        cl, r12b

.epilogue:                            ; functions typically have an epilogue
	jmp        r10                    ; jump to the return address

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ hhTransform ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

hhTransform:
	mov        r11d, ebx              ; H := B xor C xor D
	xor        r11d, ecx
	xor        r11d, edx

	lea        r13d, [3*r8d + 5]      ; g := (3×i + 5) mod 16
	and        r13d, 0x0F

	lea        r12, [rel X]           ; A += H (b,c,d) + X + (UINT4)(block[g]);
	add        r11d, [r12 + 4*r8]
	add        r11d, dword [rsi + 4*r13]
	add        eax, r11d

	mov        r12b, cl
	mov        cl, r9b
	rol        eax, cl                ; A := leftrotate(A, s[i]);
	add        eax, ebx               ; A += B
	mov        cl, r12b

.epilogue:                            ; functions typically have an epilogue
	jmp        r10                    ; jump to the return address

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ iiTransform ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

iiTransform:
	mov        r11d, edx              ; I := C xor (B or (not D))
	not        r11d
	or         r11d, ebx
	xor        r11d, ecx

	mov        r13d, r8d              ; g := (7×i) mod 16
	shl        r13d, 3
	sub        r13d, r8d
	and        r13d, 0x0F

	lea        r12, [rel X]           ; A += I (b,c,d) + X + (UINT4)(block[g]);
	add        r11d, [r12 + 4*r8]
	add        r11d, dword [rsi + 4*r13]
	add        eax, r11d

	mov        r12b, cl
	mov        cl, r9b
	rol        eax, cl                ; A := leftrotate(A, s[i]);
	add        eax, ebx               ; A += B
	mov        cl, r12b

.epilogue:                            ; functions typically have an epilogue
	jmp        r10                    ; jump to the return address
