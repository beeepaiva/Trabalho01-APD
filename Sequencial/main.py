#
#####
# Created by Beatriz Paiva
#####
#

#Se necessário, instalar pelo cmd pip install imge, caso já haja, continuar...

#Importa a criação das imgens
from PIL import Image

# Constantes
#Tamanho da imgem e quantidade de interações
WIDTH, HEIGHT  = 512, 512
QTD_ITERA = 256

minX, maxX = -2.0, 2.0
minY, maxY = -2.0, 2.0

#Rotina para calcular e retornar uma cor para o conjunto
def calc_pixel(escalar, imgCalc):
    c = complex(escalar, imgCalc)
    z, count = 0, 0

    # Quando z for maior ou igual a 2 ou quando count alcançar um determinado valor máximo, que está sendo definido pelo QTD_ITERA ele para.
    while abs(z) <= 2 and count < QTD_ITERA:
        z = z * z + c   
        count += 1
    return count 

img = Image.new("RGB", (WIDTH, HEIGHT))

#Sitemas de coordenadas
#Mapeamento de escala para cada valor
#Para processar cada ponto utilizando esse for:
for y in range(HEIGHT):
    imgCalc = y * (maxY - minY) / HEIGHT  + minY 
    for x in range(WIDTH):
        escalar = x * (maxX - minX) / WIDTH  + minX
        color = calc_pixel(escalar, imgCalc)
        img.putpixel((x, y), (color % 255 * 16, color % 255 * 16, color % 255 * 16))
  
img.show()
img.save("img-mandelbrot.png")
