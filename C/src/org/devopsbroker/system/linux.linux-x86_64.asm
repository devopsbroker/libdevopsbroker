;
; linux.linux.asm - DevOpsBroker NASM file for procfs and sysfs Linux system information
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
; This file implements the following x86-64 assembly language functions for the
; org.devopsbroker.system.linux.h header file:
;
;   o uint32_t e0004944_getUnsignedInt(char *infoPath);
;   o void e0004944_getUnsignedInt(char *infoPath, uint32_t value);
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════

; syscall      rax     rdi            rsi          rdx
; ----------------------------------------------------------------------------
; SYS_READ     0x00    uint32_t fd    char *buf    size_t count
; SYS_WRITE    0x01    uint32_t fd    char *buf    size_t count
; SYS_OPEN     0x02    filename       int flags    int mode
; SYS_CLOSE    0x03    uint32_t fd    -            -
; SYS_EXIT     0x3C    int status     -            -

%define SYS_READ     0x00
%define SYS_WRITE    0x01
%define SYS_OPEN     0x02
%define SYS_CLOSE    0x03
%define SYS_EXIT     0x3C

; File Access Mode
%define O_RDONLY 	0x00              ; Open file in read-only mode
%define O_WRONLY 	0x01              ; Open file in write-only mode
%define O_RDWR      0x02              ; Open file in read-write mode

; Constants
%define ERROR_CODE      0xFFFFFFFFFFFFF000
%define EXIT_FAILURE    1
%define ZERO            0x30
%define NINE            0x39

; ═════════════════════════════ Initialized Data ═════════════════════════════

section .data               ; DX directives
	closeErrorMsg:   db    "Cannot close file", 0

; ════════════════════════════ Uninitialized Data ════════════════════════════

section .bss                ; RESX directives

; ══════════════════════════════ Assembly Code ═══════════════════════════════

section .text               ; TEXT section

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ External Resources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

extern  e0004944_printError
extern  perror

; ~~~~~~~~~~~~~~~~~~~~~~~~~ e0004944_getUnsignedInt ~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  e0004944_getUnsignedInt:function
e0004944_getUnsignedInt:
; Parameters:
;	rdi : char *infoPath
; Local Variables:
;   rax : syscall value
;   rdi : sys_open  -> const char *filename
;	      sys_write  -> unsigned int fd
;	      sys_close -> unsigned int fd
;   rsi : sys_open  -> int flags
;	      sys_read  -> char *buf
;	rdx : sys_open  -> int mode
;	      sys_read  -> size_t count
;	r12 : file descriptor
;	      number of bytes read
;	r13 : char *infoPath
;	r8  : 64-bit character buffer
;	cl  : loop counter

.prologue:                            ; functions typically have a prologue
	push       r12                    ; save r12 onto the stack
	push       r13                    ; save r13 onto the stack
	lea        rsp, [rsp - 24]        ; prepare char buf[16] on stack

	mov        r13, rdi               ; r13 = char *infoPath

.openFile:                            ; int open(filename, flags, mode);
	mov        eax, SYS_OPEN
	mov        esi, O_RDONLY
	xor        rdx, rdx
	syscall

	cmp        rax, ERROR_CODE        ; if (returnValue > -4096)
	ja         .openError
	mov        r12d, eax

.readFile:                            ; ssize_t read(fd, buf, count);
	mov        eax, SYS_READ
	mov        edi, r12d
	mov        rsi, rsp
	mov        edx, 0x10
	syscall

	cmp        rax, ERROR_CODE        ; if (returnValue > -4096)
	ja         .readError

	cmp        al, 0x0C               ; if (numBytes >= 12)
	jae        .readError

	xchg       r12d, eax              ; exhange fd with numBytes

.closeFile:                           ; int close(int fd);
	mov        edi, eax
	mov        eax, SYS_CLOSE
	syscall

	cmp        rax, ERROR_CODE        ; if (returnValue > -4096)
	ja         .closeError

.convertString:
	xor        eax, eax               ; return value = 0

	test       r12b, r12b             ; if (numBytes == 0)
	jz         .epilogue

	mov        [rsp + r12], byte 0x0  ; buf[numBytes] = '\0'
	mov        r8, [rsp]              ; load eight characters into r8

.firstChar:
	cmp        r8b, NINE              ; if (ch > '9')
	jg         .epilogue
	sub        r8b, ZERO              ; if (ch < '0')
	jl         .epilogue

	mov        al, r8b                ; returnValue = strToInt(r8b)

	shr        r8, 8                  ; if (ch == '\0')
	test       r8b, r8b
	jz         .epilogue

	mov        cl, 0x07               ; loopCounter = 7

.whileString:
	movzx      edx, r8b

	cmp        dl, NINE               ; if (ch > '9')
	jg         .epilogue
	sub        dl, ZERO               ; if (ch < '0')
	jl         .epilogue

	lea        eax, [4*eax + eax]     ; returnValue = (eax * 10) + edx
	lea        eax, [2*eax + edx]

	shr        r8, 8                  ; if (ch == '\0')
	test       r8b, r8b
	jz         .epilogue

	dec        cl                     ; if (--loopCounter != 0)
	jnz        .whileString

.loadCharBuffer:
	mov        r8d, [rsp + 8]         ; load next four characters into r8d
	mov        cl, 0x04               ; loopCounter = 4
	jmp        .whileString

.epilogue:                            ; functions typically have an epilogue
	lea        rsp, [rsp + 24]        ; unwind char buf[16] from stack
	pop        r13                    ; restore r13 register
	pop        r12                    ; restore r12 register
	ret                               ; pop return address from stack and jump there

.openError:                           ; e0004944_printError(char *infoPath, int errorNum);
	mov        rdi, r13
	mov        rsi, rax
	neg        rsi
	call       e0004944_printError

	jmp        .epilogue

.readError:                           ; e0004944_printError(char *infoPath, int errorNum);
	mov        rdi, r13
	mov        rsi, rax
	neg        rsi
	call       e0004944_printError

	mov        eax, SYS_CLOSE         ; int close(int fd);
	mov        edi, r12d
	syscall

	xor        eax, eax               ; return value = 0
	jmp        .epilogue

.closeError:                          ; perror(char *s)
	lea        rdi, [rel closeErrorMsg]
	call       perror wrt ..plt

	jmp        .convertString

; ~~~~~~~~~~~~~~~~~~~~~~~~~ e0004944_setUnsignedInt ~~~~~~~~~~~~~~~~~~~~~~~~~~

	global  e0004944_setUnsignedInt:function
e0004944_setUnsignedInt:
; Parameters:
;	rdi : char *infoPath
;	rsi : uint32_t value
; Local Variables:
;   rax : syscall valuef
;   rdi : sys_open  -> const char *filename
;	      sys_write -> unsigned int fd
;	      sys_close -> unsigned int fd
;   rsi : sys_open  -> int flags
;	      sys_write -> char *buf
;	rdx : sys_open  -> int mode
;	      sys_write -> size_t count
;	      high 32-bits of the dividend / 32-bit remainder
;	r12 : string length
;	      file descriptor
;	r13 : char *infoPath
;	r8d : constant value 0x0A
;	cl  : string length
;	xmm0 : 128-bit character buffer
;	xmm1 : character value register

.prologue:                            ; functions typically have a prologue
	push       r12                    ; save r12 onto the stack
	push       r13                    ; save r13 onto the stack
	lea        rsp, [rsp - 24]        ; prepare char buf[16] on stack

	mov        r13, rdi               ; r13 = char *infoPath

.convertUnsigned:
	pxor       xmm0, xmm0             ; clear 128-bit character buffer
	xor        edx, edx               ; clear high 32-bits of the dividend
	mov        eax, esi               ; low 32-bits of dividend = value
	mov        r8d, 0x0A              ; r8d = 10
	xor        ecx, ecx               ; strLen = 0

	cmp        esi, r8d               ; if (value < 10)
	jb         .lessThanTenValue

.whileValue:
	div        r8d                    ; eax = (eax / 10), edx = (eax % 10)

	lea        edx, [edx + ZERO]      ; ch = edx + '0'
	movd       xmm1, edx              ; xmm0.append(ch)
	por        xmm0, xmm1
	pslldq     xmm0, 1
	inc        cl                     ; strLen++

	cmp        eax, r8d               ; if (value < 10)
	jb         .lessThanTenValue

	xor        edx, edx               ; edx = 0
	jmp        .whileValue

.lessThanTenValue:
	lea        edx, [eax + ZERO]      ; ch = eax + '0'
	movd       xmm1, edx
	por        xmm0, xmm1
	inc        cl                     ; strLen++

.saveCharBuffer:
	movdqa     [rsp], xmm0            ; save characters into buf
	mov        r12d, ecx              ; r12d = strLen

.openFile:                            ; int open(filename, flags, mode);
	mov        eax, SYS_OPEN
	mov        esi, O_WRONLY
	xor        rdx, rdx
	syscall

	cmp        rax, ERROR_CODE        ; if (returnValue > -4096)
	ja         .openError

	mov        edx, r12d              ; rdx = strLen
	mov        r12d, eax              ; r12d = file descriptor

.writeFile:                           ; ssize_t write(fd, buf, count);
	mov        eax, SYS_WRITE
	mov        edi, r12d
	mov        rsi, rsp
	syscall

	cmp        rax, ERROR_CODE        ; if (returnValue > -4096)
	ja         .writeError

.closeFile:                           ; int close(int fd);
	mov        eax, SYS_CLOSE
	mov        edi, r12d
	syscall

	cmp        rax, ERROR_CODE        ; if (returnValue > -4096)
	ja         .closeError

.epilogue:                            ; functions typically have an epilogue
	lea        rsp, [rsp + 24]        ; unwind char buf[16] from stack
	pop        r13                    ; restore r13 register
	pop        r12                    ; restore r12 register
	ret                               ; pop return address from stack and jump there

.openError:                           ; e0004944_printError(char *infoPath, int errorNum);
	mov        rdi, r13
	mov        rsi, rax
	neg        rsi
	call       e0004944_printError

	jmp        .epilogue

.writeError:                          ; e0004944_printError(char *infoPath, int errorNum);
	mov        rdi, r13
	mov        rsi, rax
	neg        rsi
	call       e0004944_printError

	jmp        .closeFile

.closeError:                          ; perror(char *s)
	lea        rdi, [rel closeErrorMsg]
	call       perror wrt ..plt

	jmp        .epilogue

; ═════════════════════════════ Private Routines ═════════════════════════════
