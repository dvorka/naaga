 .model small


 public _PutPixelX
 public _GetPixelX

 SC_INDEX	equ	03c4h	; sequencer port
 MAP_MASK 	equ     02h     ; index map mask registru
 SCREEN_WIDTH   equ	80      ; 80 * 4 rows = 320
 SCREEN_SEG	equ     0a000h

 . data

 parms struc		     	; structure parms
	   dw	2 dup (?)	; save BP and IP - without name
  X	   dw	?		; pixel X position
  Y	   dw	?
  PageBase dw	?		; offset in video RAM
  Color    dw	?
 parms ends


 .code

;-----------------------------------------------------------------------------
;
; X - mode 320 x 240
;
; void PutPixelX( int X, int Y, unsigned int PageBase, int Color );
;
;-----------------------------------------------------------------------------

 _PutPixelX proc near

	push bp
	mov bp, sp
				; filling structure
	mov ax, SCREEN_WIDTH
	mul [bp+Y]		; addr of row
	mov bx, [bp+X]
	shr bx, 1
	shr bx, 1		; X/4 == offset in row
	add bx, ax		; offset in page
	add bx, [bp+PageBase]   ; point in video RAM
	mov ax, SCREEN_SEG
	mov es, ax		; point is ES:bx

	mov cl, byte ptr[bp+X]  ;
	and cl, 011b            ; count number of row we will write to
	mov ax, 0100+MAP_MASK   ; index v SC na map mask registeru
	shl ah, cl 	        ; vypocteni bitu pro zapis do defin. vrstvy
	mov dx, SC_INDEX	; nastaveni masky povolujici zapis
	out dx, ax

	mov al, byte ptr[bp+Color]
	mov es:[bx], al

	pop bp
	ret

 endp _PutPixelX

;-----------------------------------------------------------------------------
;
; X - mode 320 x 240
;
; unsigned int GetPixelX( int X, int Y, unsigned int PageBase );
;
;-----------------------------------------------------------------------------

 GC_INDEX 	equ	03ceh	; Graphic controller port
 READ_MAP	equ	04h	; index v GC na the read map register

 _GetPixelX proc near

	push bp
	mov bp, sp

	mov ax, SCREEN_WIDTH
	mul [bp+Y]		; count line number in page
	mov bx, [bp+X]
	shr bx, 1
	shr bx, 1		; X/4 = offset in row

	add bx, ax		; offset in page
	add bx, [bp+PageBase]  ; point in video RAM
	mov ax, SCREEN_SEG
	mov es, ax

	mov ah, byte ptr [bp+X]
	and ah, 011b
	mov al, READ_MAP
	mov dx, GC_INDEX
	out dx, ax

	mov al, es:[bx]
	sub ah, ah               ; konversion to unsigned int!

	pop bp
	ret

 _GetPixelX endp

 end

