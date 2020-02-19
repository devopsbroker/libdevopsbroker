;
; constants.asm - DevOpsBroker NASM file for library-wide constants
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
; This file contains library-wide constant values that can be used by any
; assembly routines in libdevopsbroker.
; -----------------------------------------------------------------------------
;

; ════════════════════════════ Include Directives ════════════════════════════


; ═══════════════════════════════ Preprocessor ═══════════════════════════════


; ══════════════════════════════ Constant Data ═══════════════════════════════

	global  bcastbyte:data 8
	global  maxInt:data 4
	global  maxUnInt:data 4

section .rodata align=16    ; DX directives

bcastbyte:	dq  0x0101010101010101
maxInt:     dd  0x7FFFFFFF
maxUnInt:   dd  0xFFFFFFFF

; 	imul       rsi, [bcastbyte]       ; broadcast ch to every byte in xmm1
;	movq       xmm1, rsi
;	punpcklqdq xmm1, xmm1
