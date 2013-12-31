 .model small

 public InitXMode320x240x256
 public CloseXMode320x240x256


 SC_INDEX	equ	03c4h	; sequencer port
 CRTC_INDEX     equ     03d4h	; CRT controller port
 MISC_OUTPUT    equ     03c2h   ; miscellaneous output register
 MAP_MASK 	equ     02h     ; index map mask registru
 SCREEN_SEG	equ     0a000h


 ; --- data for CRT controller which are diferent from mode 13 ---

 .data
				; !!! DANGER !!!

				; data rewritten from IBM manual
				; if changed monitor can be damaged

				; !!! DANGER !!!

 CRTParams label word		; label
	dw	00d06h		; vertical total
	dw	03e07h		; overflow - bit 8 of vertcal counts
	dw 	04109h		; cell height - 2 to double scan
	dw   	0ea10h		; vertical sync start
	dw	0ac11h		; vertical sync and protect cr0-cr7
	dw	0df12h		; vertical displayed
	dw	00014h		; turn off dword mode
	dw      0e715h		; vertical blank start
	dw      00616h		; vertical blank end
	dw	0e317h		; turn on byte mode

 CRTPARAMS_LENGHT	equ	(($-CRTParams)/2) 	; in bytes from here

 .code

;------------------------------------------------------------------------------
;
; Mode - X  320 x 240 x 256
;
; Initialization routine:   void InitXMode320x240x256( void );
;
;------------------------------------------------------------------------------

 InitXMode320x240x256 proc near

	push bp
	push si
	push di

	mov ax, 13h
	int 10h			; initializing mode 13 using BIOS

	mov dx, SC_INDEX
	mov ax, 0604h
	out dx, ax		; chain4 mode forbidden
				; I unchainded it => 4 pages
	mov ax, 0100h
	out dx, ax		; synchronnous reset when switching funct.

	mov dx, MISC_OUTPUT
	mov al, 0e3h
	out dx, ax		; setting 28MHz pointly and 60hz rowly

	mov dx, SC_INDEX
	mov ax, 0300h
	out dx, ax

	mov dx, CRTC_INDEX	; reprogramming crtc controller
	mov al, 11h             ; vsync end reg obsahuje register zapisu
	out dx, al		; ochranneho bitu
	inc dx			; CRT controller data register
	in  al, dx		; ziskani aktualniho vsync end reg nastaveni
	and al, 7fh		; zmena ochranneho bitu pro zapis
	out dx, al		; crtc registers
	dec dx			; crt controller index
	cld
	mov si, offset CRTParams ; label to crt parameter table
	mov cx, CRTPARAMS_LENGHT ; lenght of table

 CRT:   lodsw                   ; loading from table and out to crt port
	out dx, ax
	loop CRT
				; now clear video RAM ...
	mov dx, SC_INDEX
	mov ax, 0f02h
	out dx, ax              ; enable writing to all  rows
	mov ax, SCREEN_SEG
	mov es, ax
	sub di, di
	sub ax, ax
	mov cx, 8000h           ; size of whole video RAM
	rep stosw               ; CLEAR whole video RAM

	pop di
	pop si
	pop bp
	ret

 InitXMode320x240x256 endp


;------------------------------------------------------------------------------
;
; Mode - X  320 x 240 x 256
;
; void CloseXMode320x240x256( void ); == set text mode
;
;------------------------------------------------------------------------------

 CloseXMode320x240x256 proc near

	mov ax, 0003h
	int 10h

 CloseXMode320x240x256 endp

 end



















