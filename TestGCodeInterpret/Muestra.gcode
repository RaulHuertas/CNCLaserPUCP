;PUCP 2021, 
;Proyecto de diseño
;Código para dibujar un cuadrado con el láser
;El cuadrado va desde la diagonal (20,20)->(200, 680)
;CABECERA
G00 S0 ;//Iniciar con el láser apagado
G28 ;Home
G90 ;Coordenadas absolutas
;
G21 ;Trabajar en milímetros
G00 X20 Y20 ; Ir al inicio
G00 F10 ; Configurar velocidad de 10mm/s
G00 S100 ; Prender el laser a 100W
G01 X1200 ; Esquina 2
G01 Y1200 ; Esquina 3
G01 X20 ; Esquina 4
G01 Y20 ; Finalizar en esquina 1
G00 S0 ; Apagar el láser
G28 ; Volver el laser al home
