;PUCP 2021, 
;Proyecto de diseño
;Código para dibujar un cuadrado con el láser
;El cuadrado va desde la diagonal (20,20)->(200, 680)
G0 S0 ;//Iniciar con el láser apagado
G28 ;Home
G90 ;Coordenadas absolutas
G21 ;Trabajar en milímetros
G0 X20 Y20 ; Ir al inicio
G0 F10 ; Configurar velocidad de 10mm/s
G0 S100 ; Prender el laser a 100W
G1 X200 ; Esquina 2
G1 Y680 ; Esquina 3
G1 X20 ; Esquina 4
G1 Y20 ; Finalizar en esquina 1
G0 S0 ; Apagar el láser
G28 ; Volver el laser al home











