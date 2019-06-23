;
; httprequest.linux.asm - DevOpsBroker NASM file for HttpRequest-related functionality
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
; org.devopsbroker.lang.string.h header file:
;
;   o void a2465172_mapQueryString(HttpRequest *request);
;   o char *a2465172_urldecode(char *paramStr, int *strLen);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════

; Constants
%define ERROR_CODE    -1

; ═════════════════════════════ Initialized Data ═════════════════════════════

section .data               ; DX directives

; ════════════════════════════ Uninitialized Data ════════════════════════════

section .bss                ; RESX directives

; ══════════════════════════════ Assembly Code ═══════════════════════════════

section .text               ; TEXT section

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ External Resources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

extern  c47905f7_putFromStack

; ~~~~~~~~~~~~~~~~~~~~~~~~~ a2465172_mapQueryString ~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  a2465172_mapQueryString:function
a2465172_mapQueryString:
; Parameters:
;	rdi : HttpRequest *request
;	rsi : char *paramStr
; Local Variables:
;	al  : zero
;	cl  : loop counter
;	r8  : 64-bit character buffer
;	edx : Number of key/value pairs

.prologue:                            ; functions typically have a prologue
	push      rbp                     ; save the caller frame pointer on the stack
	mov       rbp, rsp                ; set current frame pointer to stack pointer

	test       rsi, rsi               ; if (paramStr == NULL)
	jz         .epilogue

	push       rsi                    ; put char *name onto stack
	xor        eax, eax               ; al = 0
	xor        edx, edx               ; numElements = 0

.whileString:
	mov        r8, [rsi]              ; load eight characters into r8
	mov        cl, 0x08               ; numChars = 8

.findName:
	cmp        r8b, 0x00              ; if (ch == '\0')
	je         .invalidParamStr

	cmp        r8b, 0x3D              ; if (ch == '=')
	je         .foundName

	inc        rsi                    ; paramStr++
	dec        cl                     ; numChars--
	shr        r8, 8

	test       cl, cl                 ; if (numChars == 0)
	jnz        .findName

	mov        r8, [rsi]              ; load next eight characters into r8
	mov        cl, 0x08               ; numChars = 8
	jmp        .findName

.foundName:
	mov        [rsi], al              ; ch = '\0'
	inc        edx                    ; numElements++

	inc        rsi                    ; paramStr++
	dec        cl                     ; numChars--
	shr        r8, 8
	push       rsi                    ; put char *value onto stack

	test       cl, cl                 ; if (numChars == 0)
	jnz        .findValue

	mov        r8, [rsi]              ; load next eight characters into r8
	mov        cl, 0x08               ; numChars = 8

.findValue:
	cmp        r8b, 0x00              ; if (ch == '\0')
	je         .putFromStack

	cmp        r8b, 0x26              ; if (ch == '&')
	je         .foundValue

	inc        rsi                    ; paramStr++
	dec        cl                     ; numChars--
	shr        r8, 8

	test       cl, cl                 ; if (numChars == 0)
	jnz        .findValue

	mov        r8, [rsi]              ; load next eight characters into r8
	mov        cl, 0x08               ; numChars = 8
	jmp        .findName

.foundValue:
	mov        [rsi], al              ; ch = '\0'

	inc        rsi                    ; paramStr++
	dec        cl                     ; numChars--
	shr        r8, 8
	push       rsi                    ; put char *name onto stack

	test       cl, cl                 ; if (numChars == 0)
	jnz        .findName

	mov        r8, [rsi]              ; load next eight characters into r8
	mov        cl, 0x08               ; numChars = 8
	jmp        .findName

.putFromStack:
	mov        rsi, rsp               ; elementArray = current stack pointer
	shl        edx, 1                 ; numElements *= 2
	call       c47905f7_putFromStack

.epilogue:                            ; functions typically have an epilogue
	leave                             ; mov rsp, rbp / pop rbp
	ret                               ; pop return address from stack and jump there

.invalidParamStr:
	mov        eax, ERROR_CODE        ; Set return value to ERROR_CODE
	leave                             ; movq rsp, rbp / popq rbp
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ a2465172_urldecode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  a2465172_urldecode:function
a2465172_urldecode:
; Parameters:
;	rdi : char *paramStr
;	rsi : int *strLen
; Local Variables:
;	cx  : outputBufSize (cl) / numChars (ch)
;	rdx : output buffer pointer
;	r8  : 64-bit character buffer
;	r9  : 64-bit output buffer

.prologue:                            ; functions typically have a prologue
	push       rdi                    ; put paramStr onto the stack
	mov        rdx, rdi               ; rdx = current paramStr value
	xor        r9, r9                 ; clear output buffer
	xor        eax, eax               ; strLen = 0

	mov        r8, [rdi]              ; load eight characters into r8
	add        rdi, 0x08              ; paramStr += 8
	mov        cx, 0x0800             ; numChars = 8, outputBufSize = 0

.whileChars:
	test       r8b, r8b               ; if (ch == '\0')
	jz         .epilogue

	cmp        r8b, 0x2B              ; if (ch == '+')
	je         .plusSign

	cmp        r8b, 0x25              ; if (ch == '%')
	je         .percentEncoded

	shl        r9, 8                  ; save ch in output buffer
	inc        cl                     ; outputBufSize++
	inc        eax                    ; strLen++
	mov        r9b, r8b

	cmp        cl, 0x08               ; if (outputBufSize == 8)
	jne        .manageInputBuffer

.manageOutputBuffer:
	bswap      r9                     ; save 64-bit output buffer to memory
	mov        [rdx], r9
	add        rdx, 0x08              ; output buffer pointer += 8
	xor        cl, cl                 ; outputBufSize = 0

.manageInputBuffer:
	dec        ch                     ; numChars--
	shr        r8, 8

	test       ch, ch                 ; if (numChars == 0)
	jnz        .whileChars

	mov        r8, [rdi]              ; load next eight characters into r8
	add        rdi, 0x08              ; paramStr += 8
	mov        ch, 0x08               ; numChars = 8
	jmp        .whileChars

.percentEncoded:
	shl        r9, 8                  ; save ch in output buffer
	inc        cl                     ; outputBufSize++
	inc        eax                    ; strLen++

	lea        r10, [rel $+9]         ; get the next octet block if necessary
	jmp short  getOctetBlock

	sub        r8b, 0x30              ; r8b -= '0'
	mov        r9b, r8b               ; put first character into r9b

	lea        r10, [rel $+9]         ; get the next octet block if necessary
	jmp short  getOctetBlock

	cmp        r8b, 0x3A              ; if (ch < '9')
	jb         .decimalDigit

.hexDigit:
	sub        r8b, 0x41              ; r8b = (r8b - 'A') + 10
	add        r8b, 0x0A
	jmp        .convertDigit

.decimalDigit:
	sub        r8b, 0x30              ; r8b -= '0'

.convertDigit:
	shl        r9b, 4                 ; convert hex url encoded value to decimal
	add        r9b, r8b

	cmp        cl, 0x08               ; if (outputBufSize == 8)
	jne        .manageInputBuffer
	jmp        .manageOutputBuffer

.plusSign:
	shl        r9, 8                  ; output buffer = ' '
	mov        r9b, 0x20
	inc        cl                     ; outputBufSize++
	inc        eax                    ; strLen++

	cmp        cl, 0x08               ; if (outputBufSize == 8)
	jne        .manageInputBuffer
	jmp        .manageOutputBuffer

.epilogue:                            ; functions typically have an epilogue
	movzx      rcx, cl                ; rcx = outputBufSize
	mov        [rdx+rcx], byte 0x00   ; terminate URL decoded string

	test       cl, cl                 ; if (outputBufSize == 0)
	jz         .skipWriteOutputBuf

	add        rdx, rcx

.writeOutputBuf:
	dec        rdx                    ; save partial 64-bit output buffer to memory
	dec        cl
	mov        [rdx], r9b
	shr        r9, 8

	test       cl, cl                 ; if (outputBufSize != 0)
	jnz        .writeOutputBuf

.skipWriteOutputBuf:
	mov        [rsi], eax             ; strLen = eax

	pop        rax                    ; retrieve paramStr from the stack
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ getOctetBlock ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

getOctetBlock:
; Parameters:
;	rdi : char *paramStr
;	ch  : numChars
;	r8  : 64-bit character buffer

	dec        ch                     ; numChars--
	shr        r8, 8

	test       ch, ch                 ; if (numChars == 0)
	jnz        .epilogue

.retrieveData:
	mov        r8, [rdi]              ; load next eight characters into r8
	add        rdi, 0x08              ; paramStr += 8
	mov        ch, 0x08               ; numChars = 8

.epilogue:                            ; functions typically have an epilogue
	jmp        r10                    ; jump to the return address
