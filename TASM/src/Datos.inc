.data                                                                                                                           
;   Datos de las pantallas
modoOriginal  db 0              ; modo de video con que arranco DOS
opcionActual  db 0              ; opcion marcada en el menu
colorRect     db 0              ; color que usa el procedimiento RellenarRect
cuadro        db 0
paginaInstr   db 0              ; pagina que se esta mostrando (0 o 1)

NumColores  equ 24             ; cuantos colores en la paleta
ColorFondo  equ 0              ; colores 
ColorLinea  equ 1           
ColorTexto  equ 5             
ColorAcento equ 3          
ColorVerde  equ 4 

txtTitulo     db 'MASTERMIND$'
txtTematica   db 'PROYECTO ARQUITECTURA DE COMPUTADORES$'
txtIntegr     db 'INTEGRANTE:$'
txtEnter      db 'PRESIONE ENTER$'
txtMenu       db 'MENU PRINCIPAL$'
txtOp0        db 'JUGAR$'
txtOp1        db 'DIFICULTAD$'
txtOp2        db 'INSTRUCCIONES$'
txtOp3        db 'CREDITOS$'
txtOp4        db 'SALIR$'
txtOp6        db "FIN DEL JUEGO$"
txtFlecha     db '>$'         
txtCred1      db 'DISENO Y CODIGO:$'
txtCred2      db 'JOSHUA GODINEZ BARRIOS$'
txtCred4      db 'UNIVERSIDAD NACIONAL$'
txtCred5      db 'Selecciones Dificultad$'
txtDifiF      db 'Facil$'
txtDifiN      db 'Normal$'
txtDifiD      db 'Dificil$'
msgMenu       db 'FLECHAS:MOVER  ENTER:ELEGIR  P:PRUEBA$'
msgVolver     db 'ESC PARA VOLVER          ENTER DIGITAR$'
txtInstTit    db 'INSTRUCCIONES$'
txtI1         db 'ADIVINA EL CODIGO DE FRUTAS:$'
txtI3         db 'FRUTA CORRECTA EN LUGAR CORRECTO$'
txtI4         db 'FRUTA CORRECTA EN LUGAR INCORRECTO$'
txtI5         db 'ENTER EVALUA EL INTENTO$'
txtISec      db 'SECRETO$'
txtIInt      db 'INTENTO$'
txtIRes      db 'RESULTADO: 2         2$'
txtIRegla    db 'GANAS CON 4     Y PIERDES CON 0 INTENTOS$'
msgInstr      db 'FLECHAS CAMBIAN PAGINA  ESC VOLVER$'

nivel         db 1
escala        db 1                  ; cuantas veces se agranda al dibujar


;  Datos del juego
JuegoTablaY       equ 24               ; y donde empieza el tablero (10 filas de 16 px)
JuegoTablaX      equ 24               ; x de la primera ficha de cada fila
FilasVis   equ 10               ; filas del tablero visibles a la vez
TicsAnim   equ 5                ; tics del reloj entre cuadros de animacion

; Configuracion por nivel: posicion 0 = Facil, 1 = Normal, 2 = Dificil
tblNumPos   db 4,4,5            ; posiciones del codigo
tblNumCol   db 6,6,8            ; colores (frutas) disponibles
tblRep      db 0,1,1            ; 1 = se permiten colores repetidos
tblIntentos db 12,10,1         ; intentos
numPos      db 4                ; valores del nivel en uso
numCol      db 6
permRep     db 1
maxInt      db 10

; Tablas de la partida (RT-05): todo en arreglos
secreto     db 5 dup(0)         ; codigo secreto (frutas 1..8)
intento     db 5 dup(0)         ; intento en construccion (0 = casilla vacia)
historial   db 60 dup(0)        ; intentos jugados: fila*5 + columna
historialKiwi   db 12 dup(0)        ; kiwi de cada intento
historialBanana   db 12 dup(0)        ; banana de cada intento
conteoSec   db 9 dup(0)         ; fichas sin pareja del secreto, por fruta
conteoInt   db 9 dup(0)         ; fichas sin pareja del intento, por fruta

intentoAct  db 0                ; fila que se esta jugando (base 0)
posActiva   db 0                ; casilla seleccionada
filaBase    db 0                ; primera fila visible del tablero (scroll)
baseVieja   db 0
kiwi       db 0
banana       db 0
resultado   db 0                ; 1 = victoria, 0 = derrota, 2 = abandono
semilla     dw 0
ultimoTic   dw 0
modoPrueba  db 0                ; 1 = el docente digita el secreto (F2 en el menu)
avisoOn     db 0                ; 1 = hay un aviso en la barra de estado
activa      db 0

; Variables auxiliares de los bucles de dibujo
filaIdx     db 5
colIdx      db 0
filaLoop    db 0
colLoop     db 0
filaY       dw 0
celdaX      dw 0
finX        dw 0
finY        dw 0
numBuf      db '00$'
digBuf      db '0$'

; Textos
txtFichas     db 'FICHAS$'
txtIntento    db 'INTENTO$'
txtRestan     db 'RESTAN$'
txtNivelP     db 'NIVEL$'
txtBarra      db '/$'
txtPrueba     db 'PRUEBA$'
txtPruebaMenu db 'MODO PRUEBA$'
txtGanaste    db 'GANASTE$'
txtPerdiste   db 'PERDISTE$'
txtCodigo     db 'CODIGO SECRETO$'
msgJuego      db 'FLECHAS 1-8 COLOR ENTER:OK ESC:SALIR$'
msgIncomp     db 'FALTAN FICHAS EN EL INTENTO$'
msgSecreto    db 'SELECCIONE EL CODIGO SECRETO$'
msgFin        db 'ENTER:JUGAR OTRA VEZ           M:MENU$'

tblNomNivel   dw offset txtDifiF, offset txtDifiN, offset txtDifiD

; Las 8 frutas del juego, en el orden de las teclas 1 a 8
tblFichas     dw offset spriteManzana, offset spriteNaranja
              dw offset spriteFresa, offset spriteSandia
              dw offset spriteCerezas, offset spritePera
              dw offset spritePina, offset spriteUvas





