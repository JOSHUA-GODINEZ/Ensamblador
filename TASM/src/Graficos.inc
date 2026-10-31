
.code

; Carga en la tarjeta de video los colores que se usan en el juego.
;   Recibe:   la tabla paleta y la constante NumColores
;   Devuelve: nada
;   Destruye: nada

CargarPaleta proc
    push ax                   
    push cx
    push dx
    push si
    mov dx,3C8h                 ; DX = puerto donde se indica el color inicial
    mov al,0                    ; definir desde el color 0
    out dx,al                   ; envia el numero 0 al puerto
    inc dx                      ; DX = 3C9h, el puerto de los datos de color
    mov si,offset paleta        ; SI apunta al inicio de la tabla de colores
    mov cx,NumColores*3        ; CX = cuantos numeros enviar (3 por color)
cargaPaletaEnvia:
    mov al,[si]                 ; AL = siguiente numero del arreglo
    out dx,al                   ; lo envia a la tarjeta de video
    inc si                      ; avanza al siguiente numero
    loop cargaPaletaEnvia               ; CX = CX - 1; si no es 0, repite
    pop si                      ; registros devueltos (en orden inverso)
    pop dx
    pop cx
    pop ax
    ret                         
CargarPaleta endp




; Pinta toda la pantalla con el color de fondo.
;   Recibe:   la constante ColorFondo
;   Devuelve: nada
;   Destruye: nada
LimpiarPantalla proc
    push ax
    push cx
    push di
    push es
    mov ax,0A000h               ; A000h = segmento de la memoria de video
    mov es,ax                   ; ES apunta a la pantalla
    mov di,0                    ; DI = 0: empieza en el primer pixel
    mov al,ColorFondo          ; AL = numero del color de fondo
    mov cx,64000                ; CX = cuantos pixeles hay que pintar (320 x 200) -------------
loopPixel:
    mov es:[di],al              ; pinta el pixel numero DI con el color AL ------
    inc di                      ; pasa al pixel siguiente
    loop loopPixel               ; CX = CX - 1; si no es 0, repite
    pop es
    pop di
    pop cx
    pop ax
    ret
LimpiarPantalla endp



; Pinta un rectangulo de un solo color.
;   Recibe:   AX = x, BX = y, CX = ancho, DX = alto, variable colorRect = color
;   Devuelve: nada
;   Destruye: nada
DibujarRect proc
    push ax
    push bx
    push cx
    push dx
    push di
    push es
    push ax                     ; x guardado (la multiplicacion usa AX)
    push dx                     ; alto guardado (la multiplicacion cambia DX)
    mov ax,320                  ; AX = 320 pixeles por fila
    mul bx                      ; AX = 320 * y  (inicio de la fila y)
    mov di,ax                   ; DI = inicio de esa fila
    pop dx                      ; alto recuperado
    pop ax                      ; x recuperado
    add di,ax                   ; DI = y*320 + x = posicion del primer pixel
    mov ax,0A000h               ; segmento de la pantalla ------------
    mov es,ax                   ; ES apunta a la pantalla
    mov al,colorRect            ; AL = color con que se rellena
DibujaPorFila:
    push cx                     ; ancho guardado
    push di                     ; inicio de fila guardado
DibujaPorPixel:
    mov es:[di],al              ; pinta un pixel de la fila -------------------           
    inc di                      ; pasa al pixel de la derecha
    loop DibujaPorPixel               ; CX = CX - 1; repite hasta pintar todo el ancho
    pop di                      ; inicio de fila restaurado
    pop cx                      ; ancho restaurado
    add di,320                  ; baja a la fila siguiente
    dec dx                      ; una fila menos por dibujar
    jnz DibujaPorFila                 ; si faltan filas, repite
    pop es
    pop di
    pop dx
    pop cx
    pop bx
    pop ax
    ret
DibujarRect endp


; Dibuja el primer o el segundo cuadro de un sprite, segun el turno de la animacion.
;   Recibe:   SI = direccion del cuadro 1, AX = x, BX = y, variable cuadro (0 o 1)
;   Devuelve: nada
;   Destruye: SI
DibujarAnimacion proc
    cmp cuadro,0                ; toca el cuadro 1?
    je DibujaAnimacionSprite                ; si, dibujamos ese
    add si,256                  ; no: saltamos 250 bytes al cuadro 2 --------------------------
DibujaAnimacionSprite:
    call DibujarSprite          ; dibuja el cuadro elegido
    ret
DibujarAnimacion endp


; Dibuja una imagen de 16x16, se puede agrandar y no pinta los pixeles transparentes.
;   Recibe:   SI = direccion del sprite, AX = x, BX = y, variable escala
;   Devuelve: nada
;   Destruye: nada
DibujarSprite proc
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    push bp
    push es
    mov di,ax                   ; DI = x (se guarda ahi antes de multiplicar)
    mov ax,320
    mul bx                      ; AX = 320 * y
    add di,ax                   ; DI = y*320 + x = posicion del primer pixel
    mov ax,0A000h               ; segmento de la pantalla -------------------------
    mov es,ax                   ; ES apunta a la pantalla
    xor bh,bh
    mov bl,escala               ; BL = factor de ampliacion
    mov bp,bx                   ; BP = escala
    mov dx,16                   ; DX = 16 filas en el sprite
DibujaSpriteFila:
    mov cx,16                   ; CX = 16 pixeles por fila
    push di                     ; inicio de la fila en pantalla guardado
DibujaSpritePixel:
    mov al,[si]                 ; AL = siguiente pixel del sprite
    inc si                      ; SI pasa al pixel siguiente
    cmp al,0                    ; es transparente?
    je DibujaSpriteSalta                 ; si es 0, no se pinta nada
    push cx                     ; contador de pixeles del sprite guardado
    push di                     ; posicion de este pixel guardada
    mov bx,bp                   ; BX = filas del cuadrito (escala)
DibujaSpriteCuadro:
    mov cx,bp                   ; CX = pixeles por fila del cuadrito (escala)
    push di                     ; inicio de la fila del cuadrito guardado
DibujaSpriteBorde:
    mov es:[di],al              ; pinta un pixel del cuadro    ---------------------
    inc di                      ; pasa al pixel de la derecha
    loop DibujaSpriteBorde               ; repite 'escala' veces -----------------------
    pop di                      ; vuelve al inicio de la fila del cuadrito
    add di,320                  ; baja a la fila siguiente del cuadrito
    dec bx                      ; una fila del cuadrito menos
    jnz DibujaSpriteCuadro               ; si faltan filas, repite
    pop di                      ; posicion de este pixel restaurada
    pop cx                      ; contador de pixeles restaurado
DibujaSpriteSalta:
    add di,bp                   ; avanza 'escala' pixeles a la derecha
    loop DibujaSpritePixel               ; repite hasta terminar los 16 pixeles
    pop di                      ; vuelve al inicio de la fila del sprite
    mov cx,bp                   ; baja 'escala' filas de pantalla
DibujaSpriteBaja:
    add di,320
    loop DibujaSpriteBaja
    dec dx                      ; una fila del sprite menos
    jnz DibujaSpriteFila                 ; si faltan filas, repite -------------------
    pop es
    pop bp
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
DibujarSprite endp


; Escribe un texto en pantalla con el color indicado.
;   Recibe:   SI = direccion del texto, DH = fila, DL = columna, BL = color
;   Devuelve: nada
;   Destruye: nada
ImprimirTexto proc
    push ax
    push bx
    push cx
    push dx
    push si
    mov bh,0                    ; pagina de video 0
    mov ah,2                    ; funcion 2 de int 10h
    int 10h                     ; el cursor va a la fila DH, columna DL (una sola vez)
ImprimirTextoLetra:
    mov al,[si]                 ; AL = letra actual del texto
    cmp al,'$'                  ; es el simbolo de fin?
    je ImprimirTextoFin                   ; si es $, termina
    mov ah,0Eh                  ; funcion 0Eh: escribe la letra y avanza el cursor
    int 10h                     ; dibuja AL con el color BL
    inc si                      ; pasa a la letra siguiente
    jmp ImprimirTextoLetra  
    ImprimirTextoFin:              ; repite 
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
ImprimirTexto endp


; Dibuja el titulo arriba y una linea debajo.
;   Recibe:   SI = direccion del texto del titulo
;   Devuelve: nada
;   Destruye: nada
DibujarEncabezado proc
    push ax
    push bx
    push cx
    push dx
    push si                     ; 'Escribir' cambia SI, lo guarda
    mov colorRect,ColorLinea   ; la linea sera de este color
    mov ax,0                    ; x = 0
    mov bx,20                   ; y = 18
    mov cx,320                  ; ancho = toda la pantalla
    mov dx,3                    ; alto = 2 pixeles
    call DibujarRect   
    mov dh,1                   ; fila 24 (la ultima)
    mov dl,1                    ; columna 1
    mov bl,ColorAcento           ; dibuja la linea
    call ImprimirTexto
    pop si                      ; SI recuperado
    pop dx
    pop cx
    pop bx
    pop ax
    ret
DibujarEncabezado endp


; Dibuja la linea de abajo y escribe en ella un mensaje de ayuda.
;   Recibe:   SI = direccion del mensaje
;   Devuelve: nada
;   Destruye: nada
DibujarBarra proc
    push ax
    push bx
    push cx
    push dx
    push si                     ; SI guardado: trae la direccion del mensaje
    mov colorRect,ColorLinea   ; color de la linea
    mov ax,0                    ; x = 0
    mov bx,185                  ; y = 188
    mov cx,320                  ; ancho = toda la pantalla
    mov dx,3                    ; alto = 2 pixeles
    call DibujarRect           ; dibuja la linea
    mov dh,24                   ; fila 24 (la ultima)
    mov dl,1                    ; columna 1
    mov bl,ColorAcento         ; color naranja 
    call ImprimirTexto          ; escribe el mensaje
    pop si 
    pop dx
    pop cx
    pop bx
    pop ax
   
    ret
DibujarBarra endp




; Dibuja la pantalla completa del menu principal con sus opciones y las frutas.
;   Recibe:   variable opcionActual (la opcion que se ve marcada)
;   Devuelve: nada (deja la variable escala en 2)
;   Destruye: nada
DibujarMenuPrincipal proc
    push ax
    push bx
    push dx
    push si
    call LimpiarPantalla                    ; pantalla borrada
    mov si,offset txtTitulo    
    call DibujarEncabezado                  ; titulo y linea de arriba
    Escribir 1,25,ColorAcento,txtMenu      ; subtitulo del menu
    call DibujarOpcionesMP  


        mov escala,2                ; los sprites del menu van en tamano normal (16x16)     --------------
    PonerSprite spriteManzana,5,150  ; fila de 8 sprites, uno cada 24 pixeles
    PonerSprite spriteNaranja,50,150
    PonerSprite spriteFresa,90,150
    PonerSprite spriteSandia,130,150
    PonerSprite spriteCerezas,170,150
    PonerSprite spritePera,210,150
    PonerSprite spritePina,250,150
    PonerSprite spriteUvas,285,150


    mov si,offset msgMenu       ; SI = mensaje de ayuda
    call DibujarBarra           ; lo dibuja abajo
    pop si
    pop dx
    pop bx
    pop ax
    ret
DibujarMenuPrincipal endp


; Dibuja las 5 opciones del menu principal y marca la elegida.
;   Recibe:   variable opcionActual
;   Devuelve: nada
;   Destruye: nada
DibujarOpcionesMP proc
    push ax
    push bx
    push cx
    push dx
    push si
    OpcionMP 0,6,txtOp0           ; opcion 0 en la fila 8
    OpcionMP 1,8,txtOp1          ; opcion 1 en la fila 10
    OpcionMP 2,10,txtOp2          ; opcion 2 en la fila 12
    OpcionMP 3,12,txtOp3          ; opcion 3 en la fila 14
    OpcionMP 4,14,txtOp4          ; opcion 4 en la fila 16
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
DibujarOpcionesMP endp

; Dibuja las 3 opciones de dificultad y marca la elegida.
;   Recibe:   variable opcionActual
;   Devuelve: nada
;   Destruye: nada
DibujarOpcionesD proc
    push ax
    push bx
    push cx
    push dx
    push si
    OpcionD 0,9,txtDifiF           ; opcion 0 en la fila 8
    OpcionD 1,12,txtDifiN          ; opcion 1 en la fila 10
    OpcionD 2,15,txtDifiD          ; opcion 2 en la fila 12
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
DibujarOpcionesD endp

; Muestra la pantalla de inicio con las frutas animadas y espera que se presione ENTER.
;   Recibe:   nada
;   Devuelve: nada (deja la variable escala en 1)
;   Destruye: nada
PantallaPresentacion proc
    push ax
    push bx
    push cx
    push dx
    push si
    call LimpiarPantalla        ; pantalla borrada
    Escribir 1,15,ColorTexto,txtTitulo     ; titulo grande
    Escribir 4,1,ColorAcento,txtTematica   ; tematica
    Escribir 14,14,ColorTexto,txtIntegr     ; integrantes
    Escribir 16,9,ColorTexto,txtCred2     ; primer integrante
    Escribir 22,13,ColorAcento,txtEnter    ; instruccion

    mov escala,3                ; los sprites se dibujan 3 veces mas grandes (48x48)

PresentacionAnimar:
    mov colorRect,ColorFondo    ; borramos los dos sprites anteriores
    mov ax,96                   ; x = 96 (x del sprite izquierdo)
    mov bx,50                   ; y = 50 (la misma y que los sprites)
    mov cx,128                  ; ancho = 128 (de x=96 hasta x=224)
    mov dx,48                   ; alto = 48 (16 * escala)
    call DibujarRect

    Animar spritePera,96,50     ; sprite izquierdo
    Animar spriteManzana,176,50    ; sprite derecho   ; sprite derecho (x = 176)

    xor cuadro,1                ; cambia de cuadro
    call Pausa                  ; espera unos 4 tics

    mov ah,1                    ; hay alguna tecla?
    int 16h
    jz PresentacionAnimar                ; sin tecla: seguimos animando
    mov ah,0                    ; la leemos
    int 16h
    cmp al,13                   ; fue ENTER?
    jne PresentacionAnimar               ; si no, seguimos animando

    mov escala,1                ; volvemos al tamano normal para las demas pantallas
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
PantallaPresentacion endp


; Deja moverse por el menu principal con las flechas y devuelve la opcion elegida.
;   Recibe:   nada
;   Devuelve: AL = opcion elegida (0 a 4), o 5 si se presiono ESC
;   Destruye: AX
MenuPrincipal proc
    mov opcionActual,0          ; primera opcion marcada
    call DibujarMenuPrincipal            ; menu dibujado UNA sola vez
    call MostrarPrueba                  ; aviso de modo prueba (si esta activo)
MpLeer:
    mov ah,0                    ; esperamos una tecla
    int 16h                     ; AL = ASCII, AH = codigo de exploracion
    cmp al,13                   ; es ENTER
    je MpAcepta
    cmp al,27                   ; es ESC
    je MpCancela
    cmp al,'p'                   ; F2: activar o quitar el modo prueba
    je MpPrueba
    cmp al,'P'                   ; F2: activar o quitar el modo prueba
    je MpPrueba
    cmp al,0                    ; AL = 0 significa tecla especial (flechas)
    jne MpLeer                 ; otra tecla normal: se ignora
    cmp ah,72                   ; flecha arriba
    je MpArriba
    cmp ah,80                   ; flecha abajo
    je MpAbajo
    jmp MpLeer
MpPrueba:
    xor modoPrueba,1            ; alterna 0 y 1
    call MostrarPrueba
    jmp MpLeer
MpArriba:
    cmp opcionActual,0          ; ya estamos en la primera opcion?
    je MpLeer
    dec opcionActual            ; marcar opción anterior
   call DibujarOpcionesMP           ; solo se redibuja la flecha
    jmp MpLeer
MpAbajo:
    cmp opcionActual,4          ; ya estamos en la ultima opcion?
    jae MpLeer
    inc opcionActual            ; marcar opción siguiente
     call DibujarOpcionesMP            ; solo se redibuja la flecha
    jmp MpLeer
MpAcepta:
    mov al,opcionActual         ; devolvemos la opcion elegida
    ret
MpCancela:
    mov al,5                  ; 5 significa "se presiono ESC"
    ret
MenuPrincipal endp






; Deja elegir el nivel de dificultad con las flechas.
;   Recibe:   variable nivel (el nivel que aparece marcado al abrir)
;   Devuelve: AL = nivel elegido (0 a 2), o 255 si se presiono ESC
;   Destruye: AX
MenuDificultad proc
    mov al,nivel                ; abrimos en el nivel que esta activo ahora
    mov opcionActual,al         ; esa opcion queda marcada
    call PantallaDificultad     ; menu dibujado UNA sola vez
DifiLeer:
    mov ah,0                    ; esperamos una tecla
    int 16h                     ; AL = ASCII, AH = codigo de exploracion
    cmp al,13                   ; es ENTER?
    je DifiAcepta
    cmp al,27                   ; es ESC?
    je DifiCancela
    cmp al,0                    ; AL = 0 significa tecla especial (flechas)
    jne DifiLeer                ; otra tecla normal: se ignora
    cmp ah,72                   ; flecha arriba
    je DifiArriba
    cmp ah,80                   ; flecha abajo
    je DifiAbajo
    jmp DifiLeer
DifiArriba:
    cmp opcionActual,0          ; ya estamos en la primera opcion?
    je DifiLeer
    dec opcionActual            ; marcar opcion anterior
    call DibujarOpcionesD       ; solo se redibujan las opciones
    jmp DifiLeer
DifiAbajo:
    cmp opcionActual,2          ; ya estamos en la ultima opcion?
    jae DifiLeer
    inc opcionActual            ; marcar opcion siguiente
    call DibujarOpcionesD       ; solo se redibujan las opciones
    jmp DifiLeer
DifiAcepta:
    mov al,opcionActual         ; devolvemos la opcion elegida (0, 1 o 2)
    ret
DifiCancela:
    mov al,255                  ; 255 significa "se presiono ESC"
    ret
MenuDificultad endp


; Muestra los nombres de quienes hicieron el juego y espera ESC para volver.
;   Recibe:   nada
;   Devuelve: nada
;   Destruye: BX, DX
PantallaCreditos proc
    push ax
    push si
    call LimpiarPantalla     
    mov si, offset txtTitulo             ; pantalla borrada
    call DibujarEncabezado                  ; titulo y linea de arriba
    Escribir 1,31,ColorAcento,txtOp3   ; palabra CREDITOS
    Escribir 5,12,ColorAcento,txtCred1      ; "Diseno y codigo:"
    Escribir 9,9,ColorTexto,txtCred2     ; primer integrante  
    Escribir 18,10,ColorAcento,txtCred4     ; universidad
    mov si,offset msgVolver     ; mensaje de ayuda
    call DibujarBarra           ; lo dibuja abajo

CreditosTecla:
    mov ah,0                    ; espera una tecla
    int 16h                     ; AL = codigo ASCII de la tecla
    cmp al,27                   ; si fue ESC (27 es su codigo)   -------------------
    jne CreditosTecla               ; si no fue ESC, se vuelve a esperar
    pop si                      ; registros restaurados
    pop ax
    ret                      
PantallaCreditos endp


; Dibuja la pantalla donde se elige la dificultad.
;   Recibe:   variable opcionActual
;   Devuelve: nada
;   Destruye: BX, DX
PantallaDificultad proc
    push ax
    push si
    call LimpiarPantalla                    ; pantalla borrada
    mov si,offset txtTitulo                    ; SI = titulo del encabezado
    call DibujarEncabezado                  ; titulo y linea de arriba
    Escribir 1,28,ColorAcento,txtOp1        ; palabra Dificultad
    Escribir 5,9,ColorAcento,txtCred5       ; texto debajo del titulo
    call DibujarOpcionesD                   ; opciones de dificultad
    mov si,offset msgVolver                 ; mensaje de ayuda
    call DibujarBarra                       ; lo dibuja abajo
    pop si                                  ; registros restaurados
    pop ax
    ret
PantallaDificultad endp



; Dibuja la primera pagina de instrucciones: las frutas y que significa cada icono.
;   Recibe:   nada
;   Devuelve: nada (deja la variable escala en 1)
;   Destruye: nada
InstrPagina1 proc
    push ax
    push bx
    push dx
    push si
    Escribir 4,6,ColorAcento,txtI1          ; objetivo del juego
    mov escala,2                            ; sprites al doble (32x32)
    PonerSprite spriteManzana,44,50               ; las 6 frutas en una fila
    PonerSprite spriteNaranja,84,50
    PonerSprite spriteFresa,124,50
    PonerSprite spriteSandia,164,50
    PonerSprite spriteCerezas,204,50
    PonerSprite spritePera,244,50
    PonerSprite spriteKiwi,5,85    
    Escribir 1,26,ColorAcento,txtInstTit         ; icono de exacto
    Escribir 12,5,ColorTexto,txtI3          ; su explicacion
    PonerSprite spriteBanana,5,120           ; icono de parcial
    Escribir 17,5,ColorTexto,txtI4          ; su explicacion
    mov escala,1                            ; volvemos al tamano normal
    Escribir 21,9,ColorAcento,txtI5         ; ENTER evalua el intento
    pop si
    pop dx
    pop bx
    pop ax
    ret
InstrPagina1 endp


; Dibuja la segunda pagina de instrucciones, con un ejemplo de intento.
;   Recibe:   nada
;   Devuelve: nada (deja la variable escala en 2)
;   Destruye: nada
InstrPagina2 proc
    push ax
    push bx
    push dx
    push si
    mov escala,2                            ; frutas al doble (32x32)
    PonerSprite spriteManzana,100,32              ; codigo secreto: 1 2 3 4
    PonerSprite spriteNaranja,140,32
    PonerSprite spriteFresa,180,32
    PonerSprite spriteSandia,220,32
    PonerSprite spriteManzana,100,72              ; intento: 1 3 2 4 (dos cambiadas)
    PonerSprite spriteFresa,140,72
    PonerSprite spriteNaranja,180,72
    PonerSprite spriteSandia,220,72

    Escribir 1,26,ColorAcento,txtInstTit         ; fruta 4: en su lugar
    Escribir 5,2,ColorAcento,txtISec       ; sprite de la fila del secreto
    Escribir 10,2,ColorAcento,txtIInt      ; sprite de la fila del intento
    Escribir 16,2,ColorTexto,txtIRes 
    PonerSprite spriteKiwi,120,115           ; fruta 1: en su lugar
    PonerSprite spriteBanana,200,115 
     
    Escribir 20,0,ColorAcento,txtIRegla   
    PonerSprite spriteKiwi,89,147
    pop si
    pop dx
    pop bx
    pop ax
    ret
InstrPagina2 endp


; Muestra las instrucciones en dos paginas y deja cambiar de pagina con las flechas.
;   Recibe:   nada
;   Devuelve: nada
;   Destruye: nada
PantallaInstrucciones proc
    push ax
    push si
    mov paginaInstr,0           ; empezamos en la pagina 1
InsDibuja:
    call LimpiarPantalla        ; pantalla borrada
    mov si,offset txtTitulo    ; SI = titulo del encabezado
    call DibujarEncabezado      ; titulo y linea de arriba
    cmp paginaInstr,0           ; que pagina toca?
    jne InsPag2
    call InstrPagina1           ; pagina 1
    jmp InsBarra
InsPag2:
    call InstrPagina2           ; pagina 2
InsBarra:
    mov si,offset msgInstr      ; mensaje de ayuda
    call DibujarBarra           ; lo dibuja abajo
InsLeer:
    mov ah,0                    ; esperamos una tecla
    int 16h                     ; AL = ASCII, AH = codigo de exploracion
    cmp al,27                   ; es ESC?
    je InsFin                   ; si, volvemos al menu
    cmp al,0                    ; AL = 0 significa tecla especial (flechas)
    jne InsLeer                 ; otra tecla normal: se ignora
    cmp ah,77                   ; flecha derecha       ----------------------
    je InsSig
    cmp ah,75                   ; flecha izquierda     ------------------------
    je InsAnt
    jmp InsLeer
InsSig:
    cmp paginaInstr,1           ; ya estamos en la ultima pagina?
    je InsLeer                 ; si, salimos
    inc paginaInstr             ; pasamos a la pagina siguiente
    jmp InsDibuja
InsAnt:
    cmp paginaInstr,0           ; ya estamos en la primera pagina?
    je InsLeer                  ; si, no hacemos nada
    dec paginaInstr             ; volvemos a la pagina anterior
    jmp InsDibuja
InsFin:
    pop si
    pop ax
    ret
PantallaInstrucciones endp











; Cambia el cuadro de la animacion cada cierto tiempo, sin detener el juego.
;   Recibe:   variables ultimoTic y cuadro, constante TicsAnim
;   Devuelve: AL = 1 si el cuadro cambio, 0 si no
;   Destruye: AX
ActualizarCuadro proc
    push cx
    push dx
    mov ah,0
    int 1Ah                     ; CX:DX = contador de tics  --------------------------
    mov ax,dx
    sub ax,ultimoTic            ; tics transcurridos
    cmp ax,TicsAnim
    jb ActuCuadroNo                    ; si pasaron menos tics, no hay cambio
    mov ultimoTic,dx            ; recordar en que tic cambio el cuadro
    xor cuadro,1                ; alterna el cuadro: 0 pasa a 1 y 1 pasa a 0    ----------------
    mov al,1                    ; AL = 1: hay que redibujar
    jmp ActuCuadroFin
ActuCuadroNo:
    mov al,0                    ; AL = 0: nada cambio
ActuCuadroFin:
    pop dx
    pop cx
    ret
ActualizarCuadro endp


; Escribe un numero de un solo digito (0 a 9).
;   Recibe:   AL = numero, DH = fila, DL = columna, BL = color
;   Devuelve: nada
;   Destruye: nada
ImprimirDigito proc
    push ax
    push si
    add al,'0'
    mov digBuf,al
    mov si,offset digBuf
    call ImprimirTexto
    pop si
    pop ax
    ret
ImprimirDigito endp


; Escribe un numero de dos digitos (00 a 99).
;   Recibe:   AL = numero, DH = fila, DL = columna, BL = color
;   Devuelve: nada
;   Destruye: nada
ImprimirNum2 proc
    push ax
    push cx
    push si
    xor ah,ah
    mov cl,10
    div cl                      ; AL = decenas, AH = unidades
    add al,'0'
    add ah,'0'
    mov numBuf,al
    mov numBuf+1,ah
    mov si,offset numBuf
    call ImprimirTexto
    pop si
    pop cx
    pop ax
    ret
ImprimirNum2 endp


; Cambia el mensaje de la barra de abajo, borrando el anterior.
;   Recibe:   SI = direccion del mensaje
;   Devuelve: nada
;   Destruye: nada
MostrarMensaje proc
    push ax
    push bx
    push cx
    push dx
    RectXY ColorFondo,0,190,320,10      ; borra el texto anterior de la barra   ---------------
    call DibujarBarra                   ; dibuja la barra con el mensaje en SI
    pop dx
    pop cx
    pop bx
    pop ax
    ret
MostrarMensaje endp


; Dibuja una casilla del tablero con su marco y su fruta; la casilla activa se ve naranja y animada.
;   Recibe:   variables filaIdx y colIdx (cual casilla dibujar)
;   Devuelve: nada
;   Destruye: nada
DibujarCelda proc
    push ax
    push bx
    push cx
    push dx
    push si
    call CalcCeldaPos
    mov celdaX,ax
    mov filaY,bx
    mov activa,0                ; por defecto no es la celda activa      ------------------
    mov al,filaIdx
    cmp al,intentoAct
    jne celdaMarco
    mov al,colIdx
    cmp al,posActiva
    jne celdaMarco
    mov activa,1                ; fila actual y posicion seleccionada     -------------------
celdaMarco:
    mov colorRect,ColorLinea    ; marco normal
    cmp activa,1
    jne celdaPinta
    mov colorRect,ColorAcento   ; marco de la celda activa
celdaPinta:
    mov ax,celdaX
    mov bx,filaY
    mov cx,16
    mov dx,16
    call DibujarRect           ; cuadrado completo del color del marco
    RectXY ColorFondo,celdaX,filaY,16,16
    mov colorRect,ColorLinea
    cmp activa,1
    jne celdaHueco
    mov colorRect,ColorAcento
celdaHueco:
    mov ax,celdaX
    mov bx,filaY
    mov cx,16
    mov dx,2
    call DibujarRect           ; lado de arriba
    mov bx,filaY
    add bx,14
    call DibujarRect           ; lado de abajo
    mov bx,filaY
    mov cx,2
    mov dx,16
    call DibujarRect           ; lado izquierdo
    mov ax,celdaX
    add ax,14
    call DibujarRect           ; lado derecho
    call ColorDeCelda           ; AL = fruta de esta celda
    or al,al
    jz celdaFin                   ; casilla vacia: solo el marco------------------------------------
    dec al
    xor ah,ah
    add ax,ax                   ; la tabla tiene palabras de 2 bytes
    mov si,ax
    mov si,tblFichas[si]        ; SI = sprite de la fruta ----------------------------------------
    mov ax,celdaX
    mov bx,filaY
    cmp activa,1
    jne celdaFija
    call DibujarAnimacion       ; la ficha activa alterna 2 cuadros     --------------------------------
    jmp celdaFin
celdaFija:
    call DibujarSprite
celdaFin:
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
DibujarCelda endp


; Vuelve a dibujar solo la casilla que se esta editando.
;   Recibe:   variables intentoAct y posActiva
;   Devuelve: nada
;   Destruye: nada
RedibujarActiva proc
    push ax
    mov al,intentoAct
    mov filaIdx,al
    mov al,posActiva
    mov colIdx,al
    call DibujarCelda
    pop ax
    ret
RedibujarActiva endp


; Vuelve a dibujar las casillas que cambiaron al presionar una tecla.
;   Recibe:   BL = posicion activa que habia antes de la tecla
;   Devuelve: nada
;   Destruye: nada
RedibujarEditadas proc
    push ax
    mov al,intentoAct
    mov filaIdx,al
    cmp bl,posActiva
    je reDibujaActual                   ; el cursor no se movio ---------------------------
    mov colIdx,bl
    call DibujarCelda           ; la celda que se dejo ya no esta activa ------------------------
reDibujaActual:
    call RedibujarActiva
    pop ax
    ret
RedibujarEditadas endp


; Dibuja una fila del tablero: numero de intento, casillas y cuantos aciertos hubo.
;   Recibe:   variable filaIdx (cual fila dibujar)
;   Devuelve: nada
;   Destruye: nada
DibujarFila proc
    push ax
    push bx
    push cx
    push dx
    push si
    
    ; --- 1. Cálculo de la coordenada Y en píxeles 
    mov al,filaIdx
    sub al,filaBase
    xor ah,ah
    mov cl,16
    mul cl
    add ax,JuegoTablaY
    mov filaY,ax                ; Y de la fila en pantalla
    
    ;  2. Conversión de píxeles Y a fila de texto (Y / 8) 
    mov cl,8
    div cl                      ; Divide AX entre 8. Cociente en AL
    mov dh,al                   ; DH = fila de texto
    mov dl,1                    ; DL = columna de texto
    mov al,filaIdx
    inc al                      ; Número de intento (base 1)
    mov bl,ColorTexto
    call ImprimirNum2
    
    ; - 3. Limpieza de retroalimentación previa y dibujo de celdas 
    RectXY ColorFondo,124,filaY,68,16   ; Borra la retroalimentación anterior --------------
    mov colLoop,0
DibuFilaColumna:
    mov al,colLoop
    mov colIdx,al
    call DibujarCelda
    inc colLoop
    mov al,colLoop
    cmp al,numPos
    jb DibuFilaColumna
    
    ; --- 4. Comprobación de estado de la fila ---
    mov al,filaIdx
    cmp al,intentoAct
    jae DibuFilaFin                  ; Si es la fila actual o futura: no dibuja retroalimentación
    
    ; --- 5. Retroalimentación: Kiwi (Aciertos exactos / Toros) ---
    PonerSprite spriteKiwi,128,filaY    ; Icono kiwi ----------------
    mov ax,filaY
    mov cl,8
    div cl                      ; Reemplazo de SHR: AX / 8 -> AL
    mov dh,al                   ; DH = fila de texto
    xor bh,bh
    mov bl,filaIdx
    mov al,historialKiwi[bx]
    mov dl,18                   ; Columna para texto de kiwi
    mov bl,ColorAcento
    call ImprimirDigito         ; Cantidad de kiwi
    
    ; --- 6. Retroalimentación: Banana (Aciertos parciales / Vacas) ---
    PonerSprite spriteBanana,160,filaY  ; Icono banana      ------------------
    mov ax,filaY
    mov cl,8
    div cl                      ; Reemplazo de SHR: AX / 8 -> AL
    mov dh,al                   ; DH = fila de texto
    xor bh,bh
    mov bl,filaIdx
    mov al,historialBanana[bx]
    mov dl,22                   ; Columna para texto de banana
    mov bl,ColorAcento
    call ImprimirDigito         ; Cantidad de banana

DibuFilaFin:
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
DibujarFila endp


; Dibuja todas las filas del tablero que se ven en pantalla.
;   Recibe:   variables filaBase y maxInt
;   Devuelve: nada
;   Destruye: nada
DibujarTablero proc
    push ax
    mov al,filaBase
    mov filaLoop,al
DibuTableroFila:
    mov al,filaLoop
    mov filaIdx,al
    call DibujarFila
    inc filaLoop
    mov al,filaLoop
    cmp al,maxInt
    jae DibuTableroFin                 ; no hay mas intentos  ------------
    mov ah,filaBase
    add ah,FilasVis
    cmp al,ah
    jb DibuTableroFila                    ; quedan filas visibles por dibujar -------------
DibuTableroFin:
    pop ax
    ret
DibujarTablero endp


; Dibuja la linea divisoria y las frutas disponibles con su numero de tecla.
;   Recibe:   variable numCol (cuantas frutas hay en este nivel)
;   Devuelve: nada
;   Destruye: nada
DibujarPanel proc
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    RectXY ColorLinea,196,24,2,160
    Escribir 3,26,ColorAcento,txtFichas
    xor di,di                   ; DI = fruta (0 a numCol-1)
DibuPanelFicha:
    mov ax,di
    mov bl,3
    div bl                      ; AL = fila (idx/3), AH = columna (idx mod 3)
    mov dl,ah
    mov dh,al
    mov al,dh
    xor ah,ah
    mov cl,5
    shl ax,cl                   ; fila * 32
    add ax,38
    mov bx,ax                   ; BX = y de la fruta
    mov al,dl
    xor ah,ah
    mov cl,40
    mul cl                      ; columna * 40
    add ax,204                  ; AX = x de la fruta
    mov si,di
    add si,si
    mov si,tblFichas[si]
    call DibujarSprite          ; dibuja la fruta
    add ax,4
    mov cl,3
    shr ax,cl
    mov dl,al                   ; DL = columna de texto del numero
    mov ax,bx
    add ax,18
    shr ax,cl
    mov dh,al                   ; DH = fila de texto del numero
    mov ax,di
    inc al                      ; numero de tecla (1 a 8)
    mov bl,ColorTexto
    call ImprimirDigito
    inc di
    mov al,numCol
    xor ah,ah
    cmp di,ax
    jb DibuPanelFicha
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
DibujarPanel endp


; Muestra el intento actual, los intentos que quedan y el nivel.
;   Recibe:   variables intentoAct, maxInt y nivel
;   Devuelve: nada
;   Destruye: nada
DibujarEstado proc
    push ax
    push bx
    push dx
    push si
    Escribir 19,25,ColorTexto,txtIntento
    mov al,intentoAct
    inc al
    cmp al,maxInt
    jbe DibuEstadoPermitido
    mov al,maxInt               ; en la derrota no pasar del maximo  ---------------
DibuEstadoPermitido:
    mov dh,19
    mov dl,33
    mov bl,ColorAcento
    call ImprimirNum2           ; intento actual
    Escribir 19,35,ColorTexto,txtBarra
    mov al,maxInt
    mov dh,19
    mov dl,36
    mov bl,ColorAcento
    call ImprimirNum2           ; intentos totales
Escribir 17,25,ColorTexto,txtRestan
    mov al,maxInt
    sub al,intentoAct           ; intentos que quedan, contando el actual
    mov dh,17
    mov dl,32
    mov bl,ColorAcento
    call ImprimirNum2
    Escribir 21,25,ColorTexto,txtNivelP
    xor bh,bh
    mov bl,nivel
    add bx,bx
    mov si,tblNomNivel[bx]      ; nombre del nivel
    mov dh,21
    mov dl,31
    mov bl,ColorAcento
    call ImprimirTexto
    pop si
    pop dx
    pop bx
    pop ax
    ret
DibujarEstado endp


; Dibuja la pantalla completa de la partida.
;   Recibe:   variable modoPrueba
;   Devuelve: nada
;   Destruye: BX, DX
DibujarJuego proc
    push si
    call LimpiarPantalla
    mov si,offset txtTitulo
    call DibujarEncabezado
     Escribir 1,34,ColorAcento,txtOp0 
    cmp modoPrueba,0
    je DibuJuegoNoPrueba
    Escribir 1,14,ColorAcento,txtPrueba     ; aviso de modo prueba
DibuJuegoNoPrueba:
    call DibujarTablero
    call DibujarPanel
    call DibujarEstado
    mov si,offset msgJuego
    call DibujarBarra
    pop si
    ret
DibujarJuego endp


; Muestra u oculta el aviso de modo prueba en el menu.
;   Recibe:   variable modoPrueba
;   Devuelve: nada
;   Destruye: nada
MostrarPrueba proc
    push ax
    push bx
    push cx
    push dx
    push si
    RectXY ColorFondo,200,24,120,8
    cmp modoPrueba,0
    je mpr_fin
    Escribir 3,29,ColorAcento,txtPruebaMenu
mpr_fin:
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
MostrarPrueba endp


; Deja digitar el codigo secreto a mano para hacer pruebas; ESC conserva el aleatorio.
;   Recibe:   variable numPos
;   Devuelve: el arreglo secreto con el codigo digitado
;   Destruye: nada
IngresarSecretoManual proc
    push ax
    push bx
    push cx
    push si
    mov si,offset msgSecreto
    call MostrarMensaje
IngresaSecretoEspera:
    mov ah,1
    int 16h
    jnz IngresaSecretoTecla
    call ActualizarCuadro       ; AL = 1 si el cuadro cambio
    cmp al,0                    ; cambio el cuadro?
    je IngresaSecretoEspera                ; si no cambio, vuelve a esperar ----------------
    call RedibujarActiva        ; animar la ficha activa  --------------------
    jmp IngresaSecretoEspera
IngresaSecretoTecla:
    mov ah,0
    int 16h
    mov bl,posActiva
    call ProcesarTecla
    cmp al,2
    je IngresaSecretoFin                   ; ESC: se queda el secreto aleatorio --------------
    cmp al,1
    jne IngresaSecretoEdita
    call IntentoCompleto
    or al,al
    jz IngresaSecretoEspera                ; faltan fichas: seguir armando
    xor si,si                   ; copiar el intento al secreto ---------------------
    mov cl,numPos
    xor ch,ch
IngresaSecretoCopia:
    mov al,intento[si]
    mov secreto[si],al
    inc si
    loop IngresaSecretoCopia
    jmp IngresaSecretoFin
IngresaSecretoEdita:
    call RedibujarEditadas
    jmp IngresaSecretoEspera
IngresaSecretoFin:
    call LimpiarIntento
    mov posActiva,0
    pop si
    pop cx
    pop bx
    pop ax
    ret
IngresarSecretoManual endp


; Controla una partida completa, desde el primer intento hasta ganar, perder o abandonar.
;   Recibe:   variables nivel y modoPrueba
;   Devuelve: AL = 1 si gano, 0 si perdio, 2 si abandono con ESC
;   Destruye: AX
JugarPartida proc
    push bx
    push cx
    push dx
    push si
    mov escala,1                ; las fichas van en tamano normal
    call AplicarNivel
    call LimpiarHistorial
    call LimpiarIntento
    mov intentoAct,0
    mov posActiva,0
    mov filaBase,0
    mov avisoOn,0
    call GenerarSecreto
    call DibujarJuego
    cmp modoPrueba,0
    je JugarEspera
    call IngresarSecretoManual  ;Se digita el secreto -----------
    call DibujarJuego
JugarEspera:
    mov ah,1                    ; hay tecla?
    int 16h
    jnz JugarTecla
    call ActualizarCuadro       ; AL = 1 si el cuadro cambio
    cmp al,0                    ; cambio el cuadro?
    je JugarEspera                ; si no cambio, vuelve a esperar
    call RedibujarActiva
    jmp JugarEspera
JugarTecla:
    mov ah,0
    int 16h                     ; AL = ASCII, AH = codigo de exploracion
    mov bl,posActiva            ; recordar la posicion anterior     -----------------
    call ProcesarTecla
    cmp al,2
    je JugarAbandono
    cmp al,1
    je JugarEnter
    call RedibujarEditadas      ; solo se redibujan las celdas que cambiaron ------
    cmp avisoOn,0
    je JugarEspera
    mov avisoOn,0               ; cualquier tecla quita el aviso anterior  --------------------
    mov si,offset msgJuego
    call MostrarMensaje
    jmp JugarEspera
JugarEnter:
    call IntentoCompleto        ; un intento incompleto no gasta turno ----------------------
    or al,al
    jnz JugarEvalua
    mov si,offset msgIncomp
    call MostrarMensaje
    mov avisoOn,1
    jmp JugarEspera
JugarEvalua:
    call EvaluarIntento
    call GuardarHistorial
    mov al,intentoAct
    mov filaIdx,al              ; fila que se acaba de evaluar ---------------------
    inc intentoAct              ; ahora esa fila es historial
    mov al,kiwi
    cmp al,numPos
    je JugarVictoria              ; todos kiwi: gano --------------------
    mov al,intentoAct
    cmp al,maxInt
    jae JugarDerrota              ; se acabaron los intentos ------------------------
    call LimpiarIntento         ; la partida sigue: nueva fila vacia -----------------
    mov posActiva,0
    mov al,filaBase
    mov baseVieja,al
    mov al,intentoAct
    cmp al,FilasVis
    jb JugarIntentoMenorA10
    sub al,FilasVis-1           ; la fila actual queda de ultima visible --------------
    jmp JugarIntentoMayorA10
JugarIntentoMenorA10:
    xor al,al
JugarIntentoMayorA10:
    mov filaBase,al
    cmp al,baseVieja
    jne JugarTablero                 ; el tablero se desplazo: redibujar todo -------------------
    call DibujarFila            ; fila recien evaluada, con su retroalimentacion
    mov al,intentoAct
    mov filaIdx,al
    call DibujarFila            ; nueva fila actual ------------------------
    jmp JugarEstado
JugarTablero:
    call DibujarTablero
JugarEstado:
    call DibujarEstado
    jmp JugarEspera
JugarVictoria:
    call DibujarFila            ; mostrar la fila ganadora -------------------
    mov resultado,1
    call Pausa                  ; dar tiempo a verla
    jmp JugarFin
JugarDerrota:
    call DibujarFila
    mov resultado,0
    call Pausa
    jmp JugarFin
JugarAbandono:
    mov resultado,2
JugarFin:
    mov al,resultado
    pop si
    pop dx
    pop cx
    pop bx
    ret
JugarPartida endp


; Muestra el codigo secreto con una animacion distinta si el jugador gano o perdio.
;   Recibe:   AL = 1 si gano, 0 si perdio
;   Devuelve: AL = 0 para jugar otra vez, 1 para volver al menu
;   Destruye: AX
PantallaFin proc
    push bx
    push cx
    push dx
    push si
    mov resultado,al
    mov escala,2                ; frutas al doble (32x32) ---------------------------------
    call LimpiarPantalla
    mov si,offset txtTitulo
    call DibujarEncabezado
    Escribir 1,26,ColorAcento,txtOp6 
    cmp resultado,1
    jne PantallaFinTitulo
    Escribir 4,16,ColorAcento,txtGanaste
    jmp PantallaFinCodigo
PantallaFinTitulo:
    Escribir 4,14,ColorTexto,txtPerdiste
PantallaFinCodigo:
    Escribir 6,13,ColorTexto,txtCodigo
    mov si,offset msgFin
    call DibujarBarra
PantallaFinDibuja:
    RectXY ColorFondo,48,64,224,72      ; borra la zona de las frutas    -------------
    mov ax,84                   ; x inicial para 4 frutas -------------
    cmp numPos,4
    je PantallaFinResultado
    mov ax,64                   ; x inicial para 5 frutas ---------------------
PantallaFinResultado:
    mov bx,92                   ; y normal
    cmp resultado,1
    jne PantallaFinPierde
    cmp cuadro,0                ; victoria: salta de arriba a abajo   ------------------------
    je PantallaFinPosicion
    sub bx,16
    jmp PantallaFinPosicion
PantallaFinPierde:
    cmp cuadro,0                ; derrota: tiembla de izquierda a derecha ------------------------
    je PantallaFinIzquierda
    add ax,6
    
    jmp PantallaFinPosicion
PantallaFinIzquierda:  
    sub ax,6
PantallaFinPosicion:
    mov finX,ax
    mov finY,bx
    mov colLoop,0
PantallaFinFruta:
    xor bh,bh
    mov bl,colLoop
    mov al,secreto[bx]
    dec al
    xor ah,ah
    add ax,ax
    mov si,ax
    mov si,tblFichas[si]        ; sprite de la fruta del secreto
    mov ax,finX
    mov bx,finY
    call DibujarSprite          ; dibuja la fruta fija sin alternar cuadros
    add finX,40
    inc colLoop
    mov al,colLoop
    cmp al,numPos
    jb PantallaFinFruta
PantallaFinEspera:
    mov ah,1
    int 16h
    jnz PantallaFinTecla
    call ActualizarCuadro       ; AL = 1 si el cuadro cambio
    cmp al,0                    ; cambio el cuadro?
    je PantallaFinEspera                ; si no cambio, vuelve a esperar
    jmp PantallaFinDibuja               ; el cuadro cambio: redibujar la posicion
PantallaFinTecla:
    mov ah,0
    int 16h
    cmp al,13                   ; ENTER: otra partida
    je PantallaFinNuevo
    cmp al,'m'
    je PantallaFinMenu                ; M: menu
    cmp al,'M'
    je PantallaFinMenu
    jmp PantallaFinEspera
PantallaFinNuevo:
    xor al,al
    jmp PantallaFinTermina
PantallaFinMenu:
    mov al,1
PantallaFinTermina:
    mov escala,1                ; volver al tamano normal para las demas pantallas
    pop si
    pop dx
    pop cx
    pop bx
    ret
PantallaFin endp









