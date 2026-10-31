
;Macros
; muestra texto en pantalla con fila,columna,color,texto  
Escribir macro fila, columna, color, texto
    mov dh,fila                 ; DH = fila donde se escribe (0-24) 
    mov dl,columna              ; DL = columna donde se escribe (0-39)
    mov bl,color                ; BL = numero de color de la letra
    mov si,offset texto         ; SI = direccion del texto en memoria
    call ImprimirTexto          
endm


;  dibuja un sprite animado en (x,y)
Animar macro sprite, x, y
    mov si,offset sprite      ; SI = direccion del cuadro 1 de ese sprite
    mov ax,x                   ; AX = x
    mov bx,y                   ; BX = y
    call DibujarAnimacion            ; dibuja el cuadro que toque
endm


; dibuja un sprite fijo en (x,y)
PonerSprite macro sprite, x, y
    mov si,offset sprite      ; SI = direccion del sprite
    mov ax,x                   ; AX = x
    mov bx,y                   ; BX = y
    call DibujarSprite          ; lo dibuja
endm



; Menu principal opcion elegida lleva flecha y texto naranja
OpcionMP macro numero, fila, texto
    local sinFlecha
    mov colorRect,ColorFondo    ; color del fondo, para borrar la flecha 
    mov ax,88                   ; x = 88 (columna 11)
    mov bx,fila*8               ; y = la fila del texto, en pixeles
    mov cx,8                    ; ancho = 8 (una letra)
    mov dx,8                    ; alto = 8
    call DibujarRect           ; borra el lugar de la flecha ----
    Escribir fila,13,ColorTexto,texto         ; escribe el texto con el color normal
    cmp opcionActual,numero     ; es esta la opcion elegida?
    jne sinFlecha               ; si no, terminamos: texto normal y sin flecha
    Escribir fila,11,ColorAcento,txtFlecha    ; si es la elegida, dibuja '>'
    Escribir fila,13,ColorAcento,texto        ; y reescribe el texto en naranja encima
sinFlecha:
endm

; Menu Dificultad opcion elegida lleva flecha y texto naranja
OpcionD macro numero, fila, texto
    local sinFlecha
    mov colorRect,ColorFondo    ; color del fondo, para borrar la flecha anterior
    mov ax,120                 ; x = 88 (columna 11)
    mov bx,fila*8               ; y = la fila del texto, en pixeles
    mov cx,8                    ; ancho = 8 (una letra)
    mov dx,8                    ; alto = 8
    call DibujarRect           ; borra el lugar de la flecha
    Escribir fila,17,ColorTexto,texto         ; escribe el texto con el color normal
    cmp opcionActual,numero     ; es esta la opcion elegida?
    jne sinFlecha               ; si no, terminamos: texto normal y sin flecha
    Escribir fila,15,ColorAcento,txtFlecha    ; si es la elegida, dibuja '>'
    Escribir fila,17,ColorAcento,texto        ; y reescribe el texto en naranja encima
sinFlecha:
endm

; dibuja un rectangulo de un solo color ,x,y,ancho,alto 
RectXY macro color, px, py, anc, alt
    mov colorRect,color          ; color del rectangulo
    mov ax,px                   ; AX = x
    mov bx,py                   ; BX = y
    mov cx,anc                  ; CX = ancho
    mov dx,alt                  ; DX = alto
    call DibujarRect           ; lo dibuja
endm