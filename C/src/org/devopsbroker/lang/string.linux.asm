;
; string.linux.asm - DevOpsBroker NASM file for string-related functionality
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
; This file implements the following x86-64 assembly language functions for the
; org.devopsbroker.lang.string.h header file:
;
;   o int f6215943_compare(char *first, char *second);
;   o char *f6215943_copy(char *source, uint32_t length);
;   o void f6215943_copyToBuffer(char *source, char *buffer, uint32_t numBytes);
;   o bool f6215943_endsWith(const char *pattern, char *text);
;   o uint32_t f6215943_getLength(char *string);
;   o uint32_t f6215943_hashCode(const char *string);
;   o bool f6215943_isEqual(char *foo, char *bar);
;   o char *f6215943_search(char *pattern, char *text);
;   o char *f6215943_trim(char *string);
;   o char *f6215943_truncate(char *string, uint32_t maxLen);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════

; character values
%define TAB     0x09
%define SPACE   0x20

; ═════════════════════════════ Initialized Data ═════════════════════════════

section .data               ; DX directives

; ════════════════════════════ Uninitialized Data ════════════════════════════

section .bss                ; RESX directives

; ══════════════════════════════ Assembly Code ═══════════════════════════════

	section .text

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ External Resources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

extern  b196167f_addAll

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_compare ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_compare:function
f6215943_compare:
; Parameters:
;	rdi : char *first
;	rsi : char *second
; Local Variables:
;   eax : byte storage for first string
;   edx : byte storage for second string
;   ecx : loop counter variable
;	r8  : 64-bit character buffer (first)
;	r9  : 64-bit character buffer (second)

.prologue:                            ; functions typically have a prologue
	prefetcht0 [rdi]                  ; prefetch first into the CPU cache
	prefetcht0 [rsi]                  ; prefetch second into the CPU cache

	xor        eax, eax               ; set return value to zero
	xor        edx, edx               ; clear edx register

	cmp        rdi, rsi               ; if (first == second)
	je         .epilogue

	test       rdi, rdi               ; if (first == NULL)
	jz         .returnFirstNull

	test       rsi, rsi               ; if (second == NULL)
	jz         .returnSecondNull

.whileEqual:
	mov        r8, [rdi]              ; load eight characters into r8
	mov        r9, [rsi]              ; load eight characters into r9

	mov        ecx, 0x08              ; loop counter = 8
	add        rdi, rcx               ; first += 8
	add        rsi, rcx               ; second += 8

.firstChar:
	mov        al, r8b                ; al = first[i]
	mov        dl, r9b                ; dl = second[i]

	sub        ax, dx                 ; if (first[i] != second[i])
	jne        .returnNotEqual

	test       r8b, r8b               ; if (first[i] == '\0')
	jz         .epilogue
	dec        cl                     ; loop counter--

.nextChars:
	shr        r8, 8
	shr        r9, 8

	mov        al, r8b                ; al = first[i]
	mov        dl, r9b                ; dl = second[i]

	sub        ax, dx                 ; if (first[i] != second[i])
	jne        .returnNotEqual

	test       r8b, r8b               ; if (first[i] == '\0')
	jz         .epilogue

	dec        cl                     ; loop counter--
	jnz        .nextChars             ; if (cl > 0)
	jmp        .whileEqual

.returnNotEqual:                      ; set return value to (first[i] - second[i])
	cwde                              ; sign-extend AX word to EAX doubleword

.epilogue:
	ret                               ; pop return address from stack and jump there

.returnFirstNull:
	dec        eax                    ; set return value to -1
	ret                               ; pop return address from stack and jump there

.returnSecondNull:
	inc        eax                    ; set return value to 1
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_copy ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_copy:function
f6215943_copy:
; Parameters:
;	rdi : char *source
;	rsi : char *dest
; Local Variables:
;	cl  : numChars
;	r8  : 64-bit character buffer
;	r9  : temporary variable

.prologue:                            ; functions typically have a prologue
	prefetcht0 [rdi]                  ; prefetch source into the CPU cache
	mov        rax, rsi               ; return value = char *dest

	xor        ecx, ecx               ; numChars = 0
	mov        r8, [rdi]              ; load eight characters into r8
	mov        r9, r8                 ; save 64-bit buffer into r9
	add        rdi, 0x08              ; source += 8

.whileString:
	inc        cl                     ; numChars++
	test       r8b, r8b               ; if (ch == '\0')
	jz         .foundNull

	shr        r8, 8

	cmp        cl, 0x08
	jne        .whileString

.saveCharBuffer:
	mov        [rsi], r9              ; save eight characters into char *dest
	add        rsi, 0x08              ; dest += 8

.manageCharBuffer:
	xor        ecx, ecx               ; numChars = 0
	mov        r8, [rdi]              ; load eight characters into r8
	mov        r9, r8                 ; save 64-bit buffer into r9
	add        rdi, 0x08              ; source += 8
	jmp        .whileString

.foundNull:
	mov        [rsi], r9b             ; save partial character buffer into char *dest
	dec        cl
	inc        rsi
	shr        r9, 8

	test       cl, cl
	jnz        .foundNull

.epilogue:
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_copyToBuffer ~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_copyToBuffer:function
f6215943_copyToBuffer:
; Parameters:
;	rdi : char *source
;	rsi : char *buffer
;	rdx : uint32_t numBytes
; Local Variables:
;	al  : source byte buffer

.prologue:                            ; functions typically have a prologue
	prefetcht0 [rdi]                  ; prefetch source into the CPU cache

	test       rdi, rdi               ; if (source == NULL)
	jz         .epilogue

	test       rsi, rsi               ; if (buffer == NULL)
	jz         .epilogue

	test       edx, edx               ; if (length == 0)
	jz         .epilogue

.copyToBuffer:
	dec        edx                    ; if (--numBytes == 0)
	jz         .terminateBuffer

	mov        al, [rdi]              ; copy byte from source address into al
	inc        rdi                    ; source++
	mov        [rsi], al              ; copy al into buffer address
	inc        rsi                    ; buffer++

	test       al, al                 ; if (source character != '\0')
	jnz        .copyToBuffer

.epilogue:
	ret                               ; pop return address from stack and jump there

.terminateBuffer:
	mov        [rsi], byte 0x00       ; terminate char *buffer
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_endsWith ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_endsWith:function
f6215943_endsWith:
; Parameters:
;	rdi : char *pattern
;	rsi : char *text
; Local Variables:
;	cx  : loop variable
;	rdx : add operand
;	r8  : 64-bit character buffer
;	r9  : 64-bit character buffer
;	r10 : original char *pattern value

.prologue:                            ; functions typically have a prologue
	prefetcht0 [rdi]                  ; prefetch pattern into the CPU cache
	prefetcht0 [rsi]                  ; prefetch text into the CPU cache
	mov        cl, 0x08               ; loop counter = 8
	mov        r10, rdi               ; save pattern value into r10
	xor        rax, rax               ; return value = false

	test       rdi, rdi               ; if (pattern == NULL)
	jz         .epilogue

	test       rsi, rsi               ; if (text == NULL)
	jz         .epilogue

	mov        r8, [rdi]              ; load eight characters from pattern into r8
	mov        r9, [rsi]              ; load eight characters from text into r9

.whileText:
	cmp        r8b, r9b               ; if (pattern[i] == text[j])
	je         .compareEndsWith

	test       r9b, r9b               ; if (text[j] == '\0')
	jz         .epilogue

.manageTextBuffer:
	inc        rsi                    ; text++
	shr        r9, 8

	dec        cl                     ; text loop counter--
	jnz        .whileText

	mov        r9, [rsi]              ; load eight characters from text into r9
	mov        cl, 0x08               ; text loop counter = 8
	jmp        .whileText

.compareEndsWith:
	mov        r9, [rsi]              ; load eight characters from text into r9
	mov        r11, rsi               ; save current value of text into r11
	mov        cl, 0x08               ; text loop counter = 8

.whileEqual:
	cmp        r8b, r9b               ; if (foo[i] != bar[i])
	jne        .charNotEqual

	test       r9b, r9b               ; if (foo[i] == '\0')
	jz         .returnTrue

.manageEqualsBuffers:
	inc        rdi                    ; pattern++
	inc        rsi                    ; text++
	shr        r8, 8
	shr        r9, 8
	dec        cl                     ; loop counter--
	jnz        .whileEqual

	mov        r8, [rdi]              ; load eight characters from pattern into r8
	mov        r9, [rsi]              ; load eight characters from text into r9
	mov        cl, 0x08               ; text loop counter = 8
	jmp        .whileEqual

.charNotEqual:
	test       r9b, r9b               ; if (text[j] == '\0')
	jz         .epilogue

	mov        rdi, r10               ; pattern = original value
	mov        rsi, r11               ; text = last current value
	inc        rsi                    ; increment to next text character

	mov        r8, [rdi]              ; load eight characters from pattern into r8
	mov        r9, [rsi]              ; load eight characters from text into r9
	mov        cl, 0x08               ; text loop counter = 8
	jmp        .whileText

.returnTrue:
	inc        al                     ; set return value to true

.epilogue:
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_getLength ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_getLength:function
f6215943_getLength:
; Parameters:
;	rdi : char *string
; Local Variables:
;	xmm0 : 128-bit character buffer
;	xmm1 : all zeroes SSE2 register
;	ecx  : bitmask register

.prologue:                            ; functions typically have a prologue
	xor        eax, eax               ; return value = 0
	pxor       xmm1, xmm1             ; zero the comparison register

	test       rdi, rdi               ; if (foo == NULL)
	jz         .epilogue

.firstChunk:
	movdqu     xmm0, [rdi]            ; unaligned read of 16 bytes into xmm0
	pcmpeqb    xmm0, xmm1             ; look for zero byte
	pmovmskb   ecx, xmm0              ; create 16-bit bitmask from xmm0

	test       ecx, ecx               ; if (bitmask != 0)
	jnz        .addBitIndexToLength

.whileString:
	add        eax, 16                ; add 16 to the string length
	movdqu     xmm0, [rdi + rax]      ; unaligned read of 16 bytes into xmm0

	pcmpeqb    xmm0, xmm1             ; look for zero byte
	pmovmskb   ecx, xmm0              ; create 16-bit bitmask from xmm0

	test       ecx, ecx               ; if (bitmask == 0)
	jz         .whileString

.addBitIndexToLength:
	bsf        ecx, ecx               ; find index of first set bit
	add        eax, ecx               ; add the bit index value to the string length

.epilogue:
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_hashCode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_hashCode:function
f6215943_hashCode:
; Parameters:
;	rdi : char *string
; Local Variables:
;	cl  : loop counter
;	rdx : 64-bit character buffer
;   r9d : temporary variable

.prologue:                            ; functions typically have a prologue
	xor        eax, eax               ; return value = 0

	test       rdi, rdi               ; if (foo == NULL)
	jz         .epilogue

.whileString:
	mov        r8, [rdi]              ; load eight characters into r8
	add        rdi, 0x08
	mov        cl, 0x08               ; numChars = 8

	test       r8b, r8b               ; if (ch == '\0')
	jz         .epilogue

.whileChars:
	movzx      r9d, r8b
	shl        eax, 3
	xor        eax, r9d

	dec        cl
	shr        r8, 8

	test       cl, cl                 ; if (numChars == 0)
	je         .whileString

	test       r8b, r8b               ; if (ch == '\0')
	jnz        .whileChars

.epilogue:
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_isEqual ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_isEqual:function
f6215943_isEqual:
; Parameters:
;	rdi : char *foo
;	rsi : char *bar
; Local Variables:
;	ecx : loop counter
;	r8  : 64-bit character buffer
;	r9  : 64-bit character buffer

.prologue:                            ; functions typically have a prologue
	xor        eax, eax               ; return value = false

	cmp        rdi, rsi               ; if (foo == bar)
	je         .returnTrue

	test       rdi, rdi               ; if (foo == NULL)
	jz         .returnFalse

	test       rsi, rsi               ; if (bar == NULL)
	jz         .returnFalse

.whileEqual:
	mov        r8, [rdi]              ; load eight characters into r8
	mov        r9, [rsi]              ; load eight characters into r9

	mov        ecx, 0x08              ; loop counter = 8
	add        rdi, rcx               ; foo += 8
	add        rsi, rcx               ; bar += 8

.firstChar:
	cmp        r8b, r9b               ; if (foo[i] != bar[i])
	jne        .returnFalse

	test       r8b, r8b               ; if (foo[i] == '\0')
	jz         .returnTrue
	dec        cl                     ; loop counter--

.nextChars:
	shr        r8, 8
	shr        r9, 8

	cmp        r8b, r9b               ; if (foo[i] != bar[i])
	jne        .returnFalse

	test       r8b, r8b               ; if (foo[i] == '\0')
	jz         .returnTrue

	dec        cl                     ; loop counter--
	jnz        .nextChars             ; if (cl > 0)
	jmp        .whileEqual

.returnTrue:
	inc        al                     ; return value = true

.returnFalse:
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_search ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_search:function
f6215943_search:
; Parameters:
;	rdi : char *pattern
;	rsi : char *text
; Local Variables:
;	rdx : original char *pattern value
;	cx  : ch = pattern loop counter, cl = text loop counter
;	r8  : 64-bit character buffer
;	r9  : 64-bit character buffer

.prologue:                            ; functions typically have a prologue
	xor        eax, eax               ; return value = NULL
	mov        rdx, rdi               ; rdx = original char *pattern value

	test       rdi, rdi               ; if (pattern == NULL)
	jz         .returnNull
	prefetcht0 [rdi]                  ; prefetch the pattern string into the CPU cache

	test       rsi, rsi               ; if (text == NULL)
	jz         .returnNull
	prefetcht0 [rsi]                  ; prefetch the text string into the CPU cache

	mov        r8, [rdi]              ; load first eight characters of pattern into r8
	mov        r9, [rsi]              ; load first eight characters of text into r9
	mov        cx, 0x0808             ; loop counters = 8

.whileText:
	cmp        r8b, r9b               ; if (pattern[i] != text[i])
	jne        .wasMatchFound

	test       r8b, r8b               ; if (pattern[i] == '\0')
	jz         .returnMatch

	shr        r8, 8
	shr        r9, 8

	inc        rdi                    ; pattern++
	dec        ch                     ; pattern loop counter--
	jnz        .checkTextBuffer       ; if (pattern loop counter > 0)

.managePatternBuffer:
	mov        r8, [rdi]              ; load next eight characters of pattern into r8
	mov        ch, 0x08               ; pattern loop counter = 8

.checkTextBuffer:
	inc        rsi                    ; text++
	dec        cl                     ; text loop counter--
	jnz        .whileText             ; if (text loop counter > 0)

.manageTextBufferA:
	mov        r9, [rsi]              ; load next eight characters of text into r9
	mov        cl, 0x08               ; text loop counter = 8
	jmp        .whileText

.wasMatchFound:
	test       r8b, r8b               ; if (pattern[i] == '\0')
	jz         .returnMatch

	test       r9b, r9b               ; if (text[i] == '\0')
	jz         .returnNull

.searchAgain:
	mov        r8, [rdx]              ; load first eight characters of pattern into r8
	mov        rdi, rdx               ; rdi = original char *pattern value
	mov        ch, 0x08               ; pattern loop counter = 8

	shr        r9, 8
	inc        rsi                    ; text++
	dec        cl                     ; text loop counter--
	jnz        .whileText             ; if (cl > 0)

.manageTextBufferB:
	mov        r9, [rsi]              ; load next eight characters of text into r9
	mov        cl, 0x08               ; text loop counter = 8
	jmp        .whileText

.returnMatch:
	mov        rax, rsi               ; return value = current char *text value

.returnNull:
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_splitWithChar ~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_splitWithChar:function
f6215943_splitWithChar:
; Parameters:
;	rdi : char *string
;	rsi : char delimiter
;	rdx : ListArray *substrList
; Local Variables:
;	cl  : loop counter
;	r8  : 64-bit character buffer
;	r9d : number of matches

.prologue:                            ; functions typically have a prologue
	push       rdi                    ; save char *string

	test       rdi, rdi               ; if (string == NULL)
	jz         .epilogue
	prefetcht0 [rdi]                  ; prefetch the string into the CPU cache

	test       rsi, rsi               ; if (delimiter == '\0')
	jz         .epilogue

	xor        r9d, r9d               ; number of matches = 0
	mov        cl, 0x08               ; loop counter = 8

	mov        r8, [rdi]              ; load first eight characters of string into r8

.whileString:
	test       r8b, r8b               ; if (string[i] == '\0')
	jz         .wasMatchFound

	cmp        r8b, sil               ; if (string[i] == delimiter)
	je         .matchFound

	shr        r8, 8
	inc        rdi                    ; string++
	dec        cl                     ; loop counter--
	jnz        .whileString           ; if (loop counter > 0)

.manageStringBufferA:
	mov        r8, [rdi]              ; load next eight characters of string into r8
	mov        cl, 0x08               ; loop counter = 8
	jmp        .whileString

.matchFound:
	inc        rdi                    ; string++
	inc        r9d                    ; number of matches++
	push       rdi                    ; save next char *string

	shr        r8, 8
	dec        cl                     ; loop counter--
	jnz        .whileString           ; if (loop counter > 0)

.manageStringBufferB:
	mov        r8, [rdi]              ; load next eight characters of string into r8
	mov        cl, 0x08               ; loop counter = 8
	jmp        .whileString

.wasMatchFound:
	test       r9d, r9d               ; if (number of matches == 0)
	jz         .epilogue

	push       r9                     ; save number of matches
	mov        rdi, rdx               ; set ListArray *listArray
	mov        rsi, rsp               ; set void **elementArray
	mov        edx, r9d               ; set uint32_t numElements

	test       r9d, 0x01              ; if (number of matches is odd)
	jnz        .addAlignedElements

	; b196167f_addAll(ListArray *listArray, void **elementArray, uint32_t numElements);
.addUnalignedElements:
	sub        rsp, 8                 ; align stack frame before calling b196167f_addAll()
	call       b196167f_addAll
	add        rsp, 8                 ; unalign stack frame after calling b196167f_addAll()

	pop        r9
	lea        rsp, [rsp+8+r9*8]      ; unwind char *string values from stack
	ret                               ; pop return address from stack and jump there

.addAlignedElements:
	call       b196167f_addAll

	pop        r9
	lea        rsp, [rsp+8+r9*8]      ; unwind char *string values from stack
	ret                               ; pop return address from stack and jump there

.epilogue:                            ; functions typically have an epilogue
	add        rsp, 8                 ; unwind char *string value from stack
	ret                               ; pop return address from stack and jump there

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_trim ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_trim:function
f6215943_trim:
; Parameters:
;	rdi : char *string
; Local Variables:
;	rsi : 64-bit character buffer
;	dx  : dl = bufSize, dh = 8
;	rcx : last character position

.prologue:                            ; functions typically have a prologue
	prefetcht0 [rdi]                  ; prefetch the string into the CPU cache
	mov        rsi, [rdi]             ; put first eight characters into rsi
	mov        rax, rdi               ; rax = char *string
	mov        dx, 0x0808             ; dh = 8, bufSize = 8

.whileSpace:
	test       sil, sil               ; if (ch == '\0')
	jz         .emptyString

.isLeadingSpace:
	cmp        sil, SPACE             ; if (ch == ' ')
	je         .manageCharBufferOne

	cmp        sil, TAB               ; if (ch == '\t')
	jne        .findLastCharacter

.manageCharBufferOne:
	; get next character
	lea        r10, [rel .whileSpace]
	jmp short  getNextCharacter

.findLastCharacter:
	mov        rax, rdi               ; rax = current value of rdi
	mov        rcx, rdi               ; lastCharPos = current value of rdi

.whileString:
	test       sil, sil               ; if (ch == '\0')
	jz         .epilogue

.isTrailingSpace:
	cmp        sil, SPACE             ; if (ch == ' ')
	je         .manageCharBufferTwo

	cmp        sil, TAB               ; if (ch == '\t')
	je         .manageCharBufferTwo

	mov        rcx, rdi               ; lastCharPos = current value of rdi

.manageCharBufferTwo:
	; get next character
	lea        r10, [rel .whileString]
	jmp short  getNextCharacter

.epilogue:                            ; functions typically have an epilogue
	inc        rcx                    ; increment lastCharPos
	cmp        rcx, rdi               ; if (lastCharPos == rdi)
	je         .return

	mov        [rcx], byte 0x00       ; set lastCharPos to '\0'

.return:
	ret                               ; pop return address from stack and jump there

.emptyString:
	mov        rax, rdi               ; return current position of rdi
	ret                               ; pop return address from stack and jump there

; ═════════════════════════════ Private Routines ═════════════════════════════

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ getNextCharacter ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

getNextCharacter:
; Parameters:
;	rdi : char *ipAddress
;	rsi : 64-bit char buffer
;	dx  : dl = bufSize, dh = 8

	shr        rsi, 8                 ; shift to the next character
	dec        dl                     ; bufSize--
	inc        rdi                    ; string++

	test       dl, dl                 ; if (bufSize == 0)
	jnz        .epilogue

	mov        rsi, [rdi]             ; load next eight characters into rsi
	mov        dl, dh                 ; bufSize = 8

.epilogue:
	jmp        r10                    ; jump to the return address

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ f6215943_truncate ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  f6215943_truncate:function
f6215943_truncate:
; Parameters:
;	rdi : char *string
;	rsi : uint32_t maxLen
; Local Variables:
;	cl  : int numChars
;	r8  : 64-bit character buffer

.prologue:                            ; functions typically have a prologue
	test       rdi, rdi               ; if (string == NULL)
	jz         .epilogue

	prefetcht0 [rdi]                  ; prefetch string into the CPU cache
	mov        cl, 0x08               ; numChars = 8
	mov        r8, [rdi]

.whileString:
	test       esi, esi               ; if (maxLen == 0)
	jz         .truncateString

	test       r8b, r8b
	jz         .epilogue

	dec        cl                     ; numChars--
	dec        esi                    ; maxLen--
	inc        rdi                    ; string++
	shr        r8, 8

	test       cl, cl
	jnz        .whileString

.manageCharBuffer:
	mov        cl, 0x08               ; numChars = 8
	mov        r8, [rdi]
	jmp        .whileString

.truncateString:
	mov        [rdi], byte 0x00       ; Null-terminate string at maxLen

.epilogue:
	ret                               ; pop return address from stack and jump there
