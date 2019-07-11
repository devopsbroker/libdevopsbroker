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
;	rdi : char *queryString
;	rsi : char *paramStr
; Local Variables:
;	r8b : input character
;	r9b : temporary variable

.prologue:                            ; functions typically have a prologue
	prefetcht0 [rdi]                  ; prefetch queryString into the CPU cache

.whileChars:
	mov        r8b, [rdi]             ; ch = *queryString
	inc        rdi                    ; queryString++

	test       r8b, r8b               ; if (ch == '\0')
	jz         .epilogue

	cmp        r8b, 0x2B              ; if (ch == '+')
	je         .plusSign

	cmp        r8b, 0x25              ; if (ch == '%')
	je         .percentEncoded

	mov        [rsi], r8b             ; put character on output stream
	inc        rsi
	jmp        .whileChars

.percentEncoded:
	mov        r8b, [rdi]             ; ch = *queryString
	inc        rdi                    ; queryString++

	sub        r8b, 0x30              ; r8b -= '0'
	mov        r9b, r8b               ; put first character into r9b

	mov        r8b, [rdi]             ; ch = *queryString
	inc        rdi                    ; queryString++

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

	mov        [rsi], r9b             ; put character on output stream
	inc        rsi
	jmp        .whileChars

.plusSign:
	mov        [rsi], byte 0x20       ; put space on output stream
	inc        rsi
	jmp        .whileChars

.epilogue:                            ; functions typically have an epilogue
	mov        [rsi], byte 0x00       ; terminate URL decoded string
	ret                               ; pop return address from stack and jump there
