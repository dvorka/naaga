
.386


IDEAL


MODEL SMALL, C


CODESEG

PUBLIC C SlowShow320x200                                ; to be seen in other modules

;- macros -----------------------------------------------------------------------------


 MACRO NORMALIZE_POINTER sg, off
                                        ; seg = seg + off/2^4
                                        ; off = off mod 2^4
  push sg
  mov  sg, off
  shr  sg, 4                            ; off /= 4
  mov  bp, sp                           ; sp nelze pouzit v ALU

  add  sg, [ss:bp]                      ; add off/4, seg
  and  off, 0Fh

  pop bp                                ; popnu seg

 ENDM NORMALIZE_POINTER


;- procs -----------------------------------------------------------------------------


 PROC NormalizePointer
  ARG @@sg:WORD, @@of:WORD

  mov ax, [@@sg]                                ; !!! musi byt [] !!!
  mov ds, ax                                    ; prepr. jen vlozi misto @@sg
  mov ax, [@@of]                                ; vyraz bp-2, ja referencuji

  NORMALIZE_POINTER dx, ax

  ret                                           ; !!! musi byt ret !!!

 ENDP NormalizePointer


;------------------------------------------------------------------------------
;
; proc for slow putting number*4 bytes to screen at A000:0000
;
;------------------------------------------------------------------------------

PROC C SlowShow320x200
 ARG originalFP_SEG:WORD, originalFP_OFF:WORD, number:WORD

        PUSH DI
        PUSH SI

        MOV  CX, [number]

        ; source

	PUSH DS
        MOV  AX, [originalFP_SEG]
        MOV  DS, AX

        MOV  SI, [originalFP_OFF]

	; destination

	PUSH ES
	MOV  AX, 0A000h
	MOV  ES, AX

	MOV  DI, 0000h


	;- wait retrace ------------------------------------------------------------------

	mov dx,3DAh
  l1:
	in al,dx
	and al,08h
	jnz l1
  l2:
	in al,dx
	and al,08h
	jz  l2

	;- wait retrace ------------------------------------------------------------------

	REP  MOVSD


	POP ES
	POP DS

	POP SI
	POP DI

	RET
ENDP


END

;----------------------------------------------------------------------------


