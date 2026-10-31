
.code

; Detiene el programa un momento, poco mas de un segundo.
;   Recibe:   nada
;   Devuelve: nada
;   Destruye: nada
Pausa proc
    push ax
    push bx
    push cx
    push dx
    mov ah,0                    ; funcion 0 de int 1Ah: leer el contador de tics -------------------------------
    int 1Ah                     ; el contador queda en CX:DX ------------------------------------------
    mov bx,dx                   ; BX = tic en que empezamos a esperar
pausaEspera:
    mov ah,0                    ; leemos el contador otra vez
    int 1Ah
    sub dx,bx                   ; DX = tics transcurridos desde el inicio
    cmp dx,20                    ; ya pasaron 4 tics?
    jb pausaEspera                ; si pasaron menos de 4, seguimos esperando ------------------
    pop dx
    pop cx
    pop bx
    pop ax
    ret
Pausa endp


; Copia la configuracion del nivel elegido a las variables de la partida.
;   Recibe:   variable nivel y las tablas de configuracion de los niveles
;   Devuelve: nada (deja listas numPos, numCol, permRep y maxInt)
;   Destruye: nada
AplicarNivel proc
    push ax
    push bx
    xor bh,bh
    mov bl,nivel                ; BX = nivel, sirve de indice en las tablas
    mov al,tblNumPos[bx]
    mov numPos,al               ; posiciones del codigo
    mov al,tblNumCol[bx]
    mov numCol,al               ; frutas disponibles
    mov al,tblRep[bx]
    mov permRep,al              ; 1 si se permiten repetidas
    mov al,tblIntentos[bx]
    mov maxInt,al               ; intentos disponibles
    pop bx
    pop ax
    ret
AplicarNivel endp


; Saca un numero al azar entre 0 y un limite.
;   Recibe:   BL = limite (debe ser mayor que 0), variable semilla
;   Devuelve: AL = numero entre 0 y BL-1 (y guarda la nueva semilla)
;   Destruye: AX
NumeroAleatorio proc
    push bx
    push cx
    push dx
    
    ; --- 1. Calcular la nueva semilla ---
    mov ax,semilla
    mov dx,25173
    mul dx                      ; DX:AX = semilla * 25173
    add ax,13849
    mov semilla,ax              ; ACTUALIZACIÓN: Se guarda el nuevo valor en memoria
    
    ; --- 2. Reemplazo de SHR: Descartar los 4 bits bajos (AX / 16) ---
    mov cx,16
    xor dx,dx                   ; Limpia DX para dividir DX:AX entre CX
    div cx                      ; AX = AX / 16 (cociente)
    
    ; --- 3. Limitar el resultado al rango deseado (AX mod BX) ---
    xor dx,dx                   ; Limpia DX para la nueva división
    xor bh,bh                   ; BX = límite superior (viene en BL)
    div bx                      ; DX = AX mod BX
    mov al,dl                   ; AL = residuo (número aleatorio final)
    
    pop dx
    pop cx
    pop bx
    ret
NumeroAleatorio endp


; Inventa el codigo secreto al azar, sin repetir frutas si el nivel no lo permite.
;   Recibe:   variables numPos, numCol y permRep
;   Devuelve: el arreglo secreto con frutas de 1 a numCol
;   Destruye: nada
GenerarSecreto proc
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    mov ah,0
    int 1Ah                     ; CX:DX = contador de tics del sistema
    mov semilla,dx              ; la semilla cambia en cada partida  --------------------------
    xor si,si                   ; SI = posicion que se esta llenando
generaSecretoPosicion:
    mov bl,numCol
    call NumeroAleatorio        ; AL = 0 a numCol-1
    inc al                      ; las frutas se numeran desde 1
    cmp permRep,1
    je generaSecreto                    ; con repetidas permitidas, cualquiera sirve
    xor di,di                   ; sin repetidas: buscar en posiciones anteriores
generaSecretoNoRepetido:
    cmp di,si
    jae generaSecreto                   ; no estaba repetida
    cmp secreto[di],al
    je generaSecretoPosicion                   ; ya salio: sortear otra fruta
    inc di
    jmp generaSecretoNoRepetido
generaSecreto:
    mov secreto[si],al          ; guardar la fruta en esta posicion
    inc si
    mov dl,numPos
    xor dh,dh
    cmp si,dx
    jb generaSecretoPosicion                   ; faltan posiciones por llenar
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
GenerarSecreto endp


; Vacia las casillas del intento que se esta armando.
;   Recibe:   nada
;   Devuelve: nada (el arreglo intento queda en ceros)
;   Destruye: nada
LimpiarIntento proc
    push cx
    push si
    mov cx,5
    xor si,si
limpiaIntento:
    mov intento[si],0
    inc si
    loop limpiaIntento
    pop si
    pop cx
    ret
LimpiarIntento endp


; Borra los intentos anteriores y sus aciertos.
;   Recibe:   nada
;   Devuelve: nada (historial, historialKiwi e historialBanana quedan en ceros)
;   Destruye: nada
LimpiarHistorial proc
    push cx
    push si
    mov cx,60
    xor si,si
limpiaHistorialAnterior:
    mov historial[si],0
    inc si
    loop limpiaHistorialAnterior
    mov cx,12
    xor si,si
limpiaHistorialFrutas:
    mov historialKiwi[si],0
    mov historialBanana[si],0
    inc si
    loop limpiaHistorialFrutas
    pop si
    pop cx
    ret
LimpiarHistorial endp


; Compara el intento con el secreto y cuenta los aciertos exactos (kiwi) y parciales (banana).
;   Recibe:   arreglos secreto e intento, variables numPos y numCol
;   Devuelve: variables kiwi (fruta y lugar correctos) y banana (fruta correcta, lugar equivocado)
;   Destruye: nada
EvaluarIntento proc
    push ax
    push bx
    push cx
    push si
    mov kiwi,0
    mov banana,0
    mov cx,9                    ; limpiar los contadores
    xor si,si
ev_clr:
    mov conteoSec[si],0
    mov conteoInt[si],0
    inc si
    loop ev_clr
    xor si,si                   ; pasada 1: kiwi
    mov cl,numPos
    xor ch,ch
ev_p1:
    mov al,secreto[si]
    mov ah,intento[si]
    cmp al,ah
    jne ev_no
    inc kiwi                   ; misma fruta en la misma posicion
    jmp ev_sig
ev_no:
    xor bh,bh
    mov bl,al
    inc conteoSec[bx]           ; fruta del secreto sin pareja
    mov bl,ah
    inc conteoInt[bx]           ; fruta del intento sin pareja
ev_sig:
    inc si
    loop ev_p1
    mov cl,numCol               ; pasada 2: banana
    xor ch,ch
    mov si,1                    ; las frutas van de 1 a numCol
ev_p2:
    mov al,conteoSec[si]
    cmp al,conteoInt[si]
    jbe ev_min
    mov al,conteoInt[si]        ; AL = el menor de los dos conteos
ev_min:
    add banana,al
    inc si
    loop ev_p2
    pop si
    pop cx
    pop bx
    pop ax
    ret
EvaluarIntento endp


; Revisa si todas las casillas del intento ya tienen fruta.
;   Recibe:   arreglo intento y variable numPos
;   Devuelve: AL = 1 si estan todas llenas, 0 si falta alguna
;   Destruye: AL
IntentoCompleto proc
    push bx
    push cx
    xor bx,bx
    mov cl,numPos
    xor ch,ch
IntentoCompletoLoop:
    cmp intento[bx],0
    je IntentoCompletoNo                    ; hay una casilla vacia  --------------------------
    inc bx
    loop IntentoCompletoLoop
    mov al,1
    jmp IntentoCompletoFin
IntentoCompletoNo:
    xor al,al
IntentoCompletoFin:
    pop cx
    pop bx
    ret
IntentoCompleto endp


; Reacciona a la tecla presionada: mueve la casilla, pone o borra una fruta, o avisa si fue ENTER o ESC.
;   Recibe:   AL = codigo ASCII y AH = codigo de exploracion de la tecla (resultado de int 16h)
;   Devuelve: AL = 0 si no paso nada especial, 1 si fue ENTER, 2 si fue ESC (y cambia intento y posActiva)
;   Destruye: AL
ProcesarTecla proc
    push bx
    cmp al,27
    je procsTeclaEsc
    cmp al,13
    je procsTeclaEnter
    cmp al,8                    ; BACKSPACE ---------
    je procsTeclaBorra
    cmp al,0
    je procsTeclaExt                   ; AL = 0: tecla especial (flechas)
    cmp al,'1'
    jb procsTeclaNada                  ; teclas no validas se ignoran  ---------
    mov bl,numCol
    add bl,'0'
    cmp al,bl
    ja procsTeclaNada                  ; numero fuera del rango de frutas ----------
    sub al,'0'                  ; ASCII -> numero de fruta
    xor bh,bh
    mov bl,posActiva
    mov intento[bx],al          ; poner la fruta en la casilla activa ----------
    jmp procsTeclaNada
procsTeclaExt:
    cmp ah,75                   ; flecha izquierda
    je procsTeclaIzquierda
    cmp ah,77                   ; flecha derecha
    je procsTeclaDerecha
    cmp ah,72                   ; flecha arriba: fruta siguiente
    je procsTeclaArriba
    cmp ah,80                   ; flecha abajo: fruta anterior
    je procsTeclaAbajo
    jmp procsTeclaNada
procsTeclaIzquierda:
    cmp posActiva,0
    je procsTeclaNada
    dec posActiva
    jmp procsTeclaNada
procsTeclaDerecha:
    mov bl,numPos
    dec bl
    cmp posActiva,bl
    jae procsTeclaNada
    inc posActiva
    jmp procsTeclaNada
procsTeclaArriba:
    xor bh,bh
    mov bl,posActiva
    mov al,intento[bx]
    inc al
    cmp al,numCol
    jbe procsTeclaSetCol
    mov al,1                    ; despues de la ultima fruta vuelve a la primera --------------
    jmp procsTeclaSetCol
procsTeclaAbajo:
    xor bh,bh
    mov bl,posActiva
    mov al,intento[bx]
    cmp al,1
    ja procsTeclaDec
    mov al,numCol               ; casilla vacia o fruta 1: salta a la ultima
    jmp procsTeclaSetCol
procsTeclaDec:
    dec al
procsTeclaSetCol:
    mov intento[bx],al
    jmp procsTeclaNada
procsTeclaBorra:
    xor bh,bh
    mov bl,posActiva
    mov intento[bx],0           ; vaciar la casilla activa
procsTeclaNada:
    xor al,al
    jmp procsTeclaFin
procsTeclaEnter:
    mov al,1
    jmp procsTeclaFin
procsTeclaEsc:
    mov al,2
procsTeclaFin:
    pop bx
    ret
ProcesarTecla endp


; Guarda el intento y sus aciertos en la fila que se acaba de jugar.
;   Recibe:   arreglo intento, variables intentoAct, numPos, kiwi y banana
;   Devuelve: nada (llena una fila de historial, historialKiwi e historialBanana)
;   Destruye: nada
GuardarHistorial proc
    push ax
    push bx
    push cx
    push si
    push di
    mov al,intentoAct
    mov bl,5
    mul bl                      ; AX = intentoAct * 5
    mov di,ax                   ; DI = inicio de la fila en el historial
    xor si,si
    mov cl,numPos
    xor ch,ch
GuardaHistorialLoop:
    mov al,intento[si]
    mov historial[di],al
    inc si
    inc di
    loop GuardaHistorialLoop
    xor bh,bh
    mov bl,intentoAct
    mov al,kiwi
    mov historialKiwi[bx],al
    mov al,banana
    mov historialBanana[bx],al
    pop di
    pop si
    pop cx
    pop bx
    pop ax
    ret
GuardarHistorial endp


; Dice que fruta va en una casilla del tablero: la del historial, la del intento actual o ninguna.
;   Recibe:   variables filaIdx, colIdx e intentoAct
;   Devuelve: AL = fruta de esa casilla (0 si esta vacia)
;   Destruye: AX
ColorDeCelda proc
    push bx
    mov al,filaIdx
    cmp al,intentoAct
    jb colorCeldaHistorial
    ja colorCeldaVacia
    xor bh,bh                   ; fila actual: leer el intento en construccion
    mov bl,colIdx
    mov al,intento[bx]
    jmp colorCeldaFin
colorCeldaHistorial:
    mov bl,5
    mul bl                      ; AX = fila * 5
    xor bh,bh
    mov bl,colIdx
    add bx,ax
    mov al,historial[bx]
    jmp colorCeldaFin
colorCeldaVacia:
    xor al,al
colorCeldaFin:
    pop bx
    ret
ColorDeCelda endp


; Calcula en que punto de la pantalla va una casilla del tablero.
;   Recibe:   variables filaIdx, colIdx y filaBase
;   Devuelve: AX = x, BX = y (esquina superior izquierda de la casilla)
;   Destruye: AX, BX
CalcCeldaPos proc
    push cx
    xor ah,ah
    mov al,colIdx
    mov cl,20
    mul cl                      ; AX = columna * 20 (16 de ficha + 4 de espacio)
    add ax,JuegoTablaX
    push ax                     ; guardar x
    mov al,filaIdx
    sub al,filaBase             ; fila dentro de la ventana visible
    xor ah,ah
    mov cl,16
    mul cl                      ; AX = fila visible * 16
    add ax,JuegoTablaY
    mov bx,ax                   ; BX = y
    pop ax                      ; AX = x
    pop cx
    ret
CalcCeldaPos endp











