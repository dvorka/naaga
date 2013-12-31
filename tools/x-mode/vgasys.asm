 .model small

 public WaitRetrace

 .data

;------------------------------------------------------------------------------
;
; Waiting for horizontal retrace
;
; void WaitRetrace( void );
;
;------------------------------------------------------------------------------

 WaitRetrace proc near
	mov dx,3DAh
  l1:
	in al,dx
	and al,08h
	jnz l1
 l2:
	in al,dx
	and al,08h
	jz  l2

 WaitRetrace endp

