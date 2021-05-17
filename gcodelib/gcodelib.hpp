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
        Comando(
            Tipo tipo,
            int lineN = 0
        ){
            this->tipo = tipo;
            int lineaOrigen = lineN;
        }
        Comando(
            Tipo tipo,
            float destinoX = 0,
            float destinoY = 0,
            int lineaOrigen = 0
        ){
            this->tipo = tipo;
            this->destinoX = destinoX;
            this->destinoY = destinoY;
            this->lineaOrigen = lineaOrigen;
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

    };

    Gcodelib();

    static int cargarArchivo(
        const QString& filename,
        std::vector<Comando>& comandos
    );
};

#endif // GCODELIB_HPP
