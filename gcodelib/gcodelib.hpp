#ifndef GCODELIB_HPP
#define GCODELIB_HPP

#include <cstdint>
#include <QString>
#include "gcodelib_global.h"
#include <vector>

class GCODELIB_EXPORT Gcodelib
{
public:
    enum Tipo: std::uint8_t
    {
        INICIO,
        HOME,
        MOV,
        POWER
    };

    struct Comando{

        Tipo tipo = Tipo::INICIO;
        float potenciaLaser = 0.0f;


        int destinoX = 0;
        int destinoY = 0;
        int lineaOrigen = 0;
        long tiempoEspera = 10;//ms


        Comando(
            Tipo tipo,
            int lineN = 0
        ){
            this->tipo = tipo;
            this->lineaOrigen = lineN;
        }
        Comando(
            Tipo tipo,
            float destinoX,
            float destinoY,
            int lineaOrigen,
            long tiempoEspera = 0
        ){
            this->tipo = tipo;
            this->destinoX = destinoX;
            this->destinoY = destinoY;
            this->lineaOrigen = lineaOrigen;
            this->tiempoEspera = tiempoEspera;
        }

        Comando(
            Tipo tipo,
            float potenciaLaser,
            float destinoX,
            float destinoY,
            int lineaOrigen,
            long tiempoEspera
        ){
            this->tipo = tipo;
            this->potenciaLaser = potenciaLaser;
            this->destinoX = destinoX;
            this->destinoY = destinoY;
            this->lineaOrigen = lineaOrigen;
            this->tiempoEspera = tiempoEspera;
        }

        Comando(
            Tipo tipo,
            float potenciaLaser,
            int lineaOrigen = 0
        ){
            this->tipo = tipo;
            this->potenciaLaser = potenciaLaser;
            this->lineaOrigen = lineaOrigen;
        }

        bool isMovQ()const{
            return (tipo == Tipo::MOV);
        }

        bool isLaserQ()const{
            return (tipo == Tipo::POWER);
        }

    };

    Gcodelib();

    static int cargarArchivo(
        const QString& filename,
        std::vector<Comando>& comandos,
        std::vector<Comando>& comandosConTiempo,
        int& lineasLeidas,
        double accMax = 10,
        double vMax = 10
    );
};

#endif // GCODELIB_HPP
