.model small                   
.stack 100h                   
jumps  

include Macros.inc 

.data
include Datos.inc 
include Paleta.inc
.code
include Graficos.inc 
include Logica.inc 



main proc
    mov ax,@data                ; AX = direccion del segmento de datos
    mov ds,ax                   ; DS apunta a los datos
    mov ah,0Fh                  ; funcion 0Fh de int 10h: leer el modo de video actual
    int 10h                     ; AL = modo actual
    mov modoOriginal,al         ; modo original guardado para restaurar al salir
    mov ax,0013h                ; AH=0 (cambiar modo), AL=13h (320x200, 250 colores)
    int 10h                     ; entra al modo grafico
    call CargarPaleta           ; define colores

inicioJuego:
    call PantallaPresentacion   ; primera pantalla

menuInicial:
    call MenuPrincipal          ; segunda pantalla: AL = opcion elegida
   
    cmp al,5                  ; se presiono ESC?
    je inicioJuego             ; volver a la presentacion

    cmp al,0                    ; se eligio JUGAR?
    je inicioJugar
   
    cmp al,1                    ; se eligio CREDITOS?
    je inicioDificultad
    
    cmp al,2                    ; se eligio CREDITOS?
    je inicioInstrucciones
  
    cmp al,3                    ; se eligio CREDITOS?
    je inicioCreditos
 
    cmp al,4                    ; se eligio SALIR
    je inicioSalir
    jmp menuInicial                ; Jugar, Nivel e Instrucciones: aun no implementadas

inicioJugar:
    call JugarPartida           ; AL = 1 victoria, 0 derrota, 2 abandono
    cmp al,2
    je menuInicial                 ; abandono: volver al menu
    call PantallaFin            ; revela el codigo con la animacion segun el resultado
    cmp al,0
    je inicioJugar                ; ENTER: otra partida sin salir
    jmp menuInicial                ; M o ESC: volver al menu

inicioDificultad:
    call MenuDificultad         ; AL = 0, 1 o 2 (nivel), o un numero mayor si fue ESC
    cmp al,2                    ; es un nivel valido (0, 1 o 2)?
    ja menuInicial                 ; si es mayor que 2 fue ESC: no se cambia el nivel
    mov nivel,al                ; guardamos el nivel elegido
    jmp menuInicial                ; volvemos al menu

    inicioInstrucciones:
    call PantallaInstrucciones  ; muestra las dos paginas
    jmp menuInicial                ; y vuelve al menu

inicioCreditos:
    call PantallaCreditos       ; creditos mostrados
    jmp menuInicial                ; retorno al menu

inicioSalir:
    mov ah,0                    ; funcion 0 de int 10h: cambiar el modo de video
    mov al,modoOriginal         ; modo guardado al inicio
    int 10h                     ; pantalla restaurada como estaba
    mov ax,4C00h                ; funcion 4Ch de DOS: terminar el programa
    int 21h                     ; regresa el control a DOS
main endp

end main                        ; fin del programa;