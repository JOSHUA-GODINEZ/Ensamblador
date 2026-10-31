 Instrucciones de Ensamblado y Ejecución

1. Abrir DOSBox.

2. Montar la carpeta "tasm\src", la cual tiene que estar guardada en C:
   mount C C:\tasm\src

3. Cambiar a la unidad montada:
   C:

4. Ensambla.
tasm pymain.asm

5.Enlaza.
tlink pymain.obj

6.Ejecuta.
pymain.exe
