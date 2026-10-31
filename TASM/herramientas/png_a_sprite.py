#!/usr/bin/env python3

import os
import re
import sys

from PIL import Image

TAM = 16
INICIO_DEFECTO = 5


def aviso(texto):
    print(texto, file=sys.stderr)


def pedir_inicio():
    while True:
        sys.stderr.write( f"[Enter = {INICIO_DEFECTO}]: ")
        sys.stderr.flush()
        linea = sys.stdin.readline()
        if linea == "":                      # no hay teclado (entrada vacia)
            return INICIO_DEFECTO
        linea = linea.strip()
        if linea == "":
            return INICIO_DEFECTO
        if linea.isdigit() and 1 <= int(linea) <= 255:
            return int(linea)
        aviso("Escriba un numero entre 1 y 255.")


def nombre_etiqueta(ruta):
    
    base = os.path.splitext(os.path.basename(ruta))[0]
    base = re.sub(r"[^A-Za-z0-9_]", "_", base)
    return "spr" + base[:1].upper() + base[1:]


def preparar(img, ruta, escalar, colores):
    """Deja la imagen en 16x16 y, si se pide, con pocos colores."""
    if img.size != (TAM, TAM):
        if not escalar:
            sys.exit(f"{ruta}: mide {img.size}, debe ser {TAM}x{TAM} "
                     f"(use --escalar para reducirla)")
        img = img.resize((TAM, TAM), Image.LANCZOS)
    if colores:
        alfa = img.getchannel("A")                      # conservar transparencia
        rgb = img.convert("RGB").quantize(colores, method=0).convert("RGB")
        img = rgb.convert("RGBA")
        img.putalpha(alfa)
    return img


def main():
    args = sys.argv[1:]
    inicio = None
    escalar = False
    colores = 0
    archivos = []
    i = 0
    while i < len(args):
        if args[i] == "--inicio":
            inicio = int(args[i + 1])
            i += 2
        elif args[i] == "--colores":
            colores = int(args[i + 1])
            i += 2
        elif args[i] == "--escalar":
            escalar = True
            i += 1
        else:
            archivos.append(args[i])
            i += 1
    if not archivos:
        print(__doc__)
        sys.exit(1)
    if inicio is None:
        inicio = pedir_inicio()

    paleta = {}        # (r, g, b) en 0-63  ->  indice de paleta
    sprites = []       # (etiqueta, filas de indices)

    for ruta in archivos:
        img = preparar(Image.open(ruta).convert("RGBA"), ruta, escalar, colores)
        filas = []
        for y in range(TAM):
            fila = []
            for x in range(TAM):
                r, g, b, a = img.getpixel((x, y))
                if a < 128:
                    indice = 0                       # transparente
                else:
                    color = (r // 4, g // 4, b // 4)  # 0-255 -> 0-63
                    if color not in paleta:
                        paleta[color] = inicio + len(paleta)
                    indice = paleta[color]
                fila.append(indice)
            filas.append(fila)
        sprites.append((nombre_etiqueta(ruta), filas))

    ultimo = inicio + len(paleta) - 1
    if paleta and ultimo > 255:
        sys.exit(f"Demasiados colores: la paleta llegaria al indice {ultimo}. "
                 f"Use --colores para limitarlos.")
    total = max(ultimo + 1, inicio)

    print("; Generado por png_a_sprite.py")
    print(f"; --- PALETA: agregar al final de 'paleta' (desde el color {inicio}) ---")
    for color, indice in sorted(paleta.items(), key=lambda p: p[1]):
        print(f";     db {color[0]},{color[1]},{color[2]}    ; color {indice}")
    print(f"; y poner  NUM_COLORES equ {total}")
    print()
    for etiqueta, filas in sprites:
        for i, fila in enumerate(filas):
            prefijo = f"{etiqueta:<12}" if i == 0 else " " * 12
            print(f"{prefijo}db " + ",".join(str(v) for v in fila))
        print()

    aviso(f"Listo: {len(sprites)} sprite(s), {len(paleta)} color(es) nuevo(s) "
          f"(indices {inicio} a {ultimo}).")
    aviso(f"NUM_COLORES debe valer {total}. "
          f"La proxima vez, empiece en el indice {total}.")


if __name__ == "__main__":
    main()
