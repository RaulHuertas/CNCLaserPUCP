#include "gcodelib.hpp"
#include <QFile>
#include <QDebug>
#include <algorithm>
#include <cmath>

Gcodelib::Gcodelib()
{

}

void buscarEnTokens(
    QStringList tokens,
    bool coordRelativeQ,
    const int oldX,
    const int oldY,
    int& newX,
    int& newY,
    int& newPower,
    int& newSpeed,
    int& newCentroX,
    int& newCentroY
){
    for(int t=1; t<tokens.size(); t++){
        auto tok = tokens[t];
        auto firstChar = tok[0];
        if(tok.length()<2){
            qDebug()<<Q_FUNC_INFO<<"token desconocido";
        }
        auto cmd = tok[0];
        auto resto = tok.mid(1);
        if(cmd=='X'){
            newX = (resto.toDouble()*10.0);
            if(coordRelativeQ){
                newX+=oldX;
            }
        }else if(cmd=='Y'){
            newY = (resto.toDouble()*10.0);
            if(coordRelativeQ){
                newY+=oldY;
            }
        }else if(cmd=='S'){//potencia laser
            newPower = resto.toDouble();
            newPower = std::max(newPower,0);
            newPower = std::min(newPower,100);
        }else if(cmd=='F'){//velocidad de desplazamiento
            newSpeed = resto.toDouble();
            newSpeed = std::max(newSpeed,0);
            newSpeed = std::min(newSpeed,500);
        }else if(cmd=='I'){
            newCentroX = oldX+(resto.toDouble()*10.0);
        }else if(cmd=='J'){
            newCentroY = oldY+(resto.toDouble()*10.0);
        }
    }
}

int Gcodelib::cargarArchivo(
    const QString& filename,
    std::vector<Comando>& comandos,
    std::vector<Comando>& comandosConTiempo,
    int& lineN,
    double accMax,
    double vMax
){
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return 1;
    }
    lineN = 0;
    int ret = 0;
    //Parámetros actuales
    bool modoCoordsRelativo = true;//por defecto se opera en relativo
    int actualX = 0;
    int actualY = 0;
    int actualPotencia = 0;
    int actualSpeed = 0;
    int newX = 0;
    int newY = 0;
    int newPotencia = 0;
    int newSpeed = 0;
    int newCentroX = 0;
    int newCentroY = 0;
    bool omitIncreaselineN = false;
    comandos.reserve(100000);
    comandos.emplace_back(Gcodelib::HOME, 0);
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QString linea(line);
        linea = linea.trimmed();//eliminar white spaces
        qDebug()<<Q_FUNC_INFO<<"A"<<linea;
        //buscar comentarios
        auto comentsStart = linea.indexOf(";");
        if(comentsStart>=0){//comentario encontrado, descartarlo
            linea = linea.left(comentsStart);
        }
        qDebug()<<Q_FUNC_INFO<<"B"<<linea;
        if(linea.length()<=0){//linea vacía
            qDebug()<<Q_FUNC_INFO<<"línea vacía"<<linea;
            lineN++;
            continue;
        }
        auto tokens = linea.split(" ");
        qDebug()<<Q_FUNC_INFO<<"EVA: "<<linea;
        auto primerToken = tokens[0];
        primerToken = primerToken.mid(1);
        //int comando = 0;
        if(linea.startsWith("%")||linea.startsWith("(")){
            //ignorar
            qDebug()<<Q_FUNC_INFO<<"Líneas mal iniciadas, ignorando"<<linea;
            lineN++;
            continue;
        }
        else if(linea.startsWith("M")){//linea vacía
            if(primerToken.compare("30")==0){
                qDebug()<<Q_FUNC_INFO<<"MCode 30, fin de programa"<<linea;
                comandos.emplace_back(Gcodelib::POWER, 0, lineN+1);
                comandos.emplace_back(Gcodelib::MOV, 0, 0, lineN+1);
            }
        }else if(linea.startsWith("G")){
            if(primerToken.compare("00")==0 || primerToken.compare("0")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 0"<<linea;
                buscarEnTokens(tokens, modoCoordsRelativo, actualX, actualY, newX, newY, newPotencia, newSpeed, newCentroX, newCentroY);

                comandos.emplace_back(Gcodelib::POWER, 0, lineN+1);
                if((newX!=actualX)||(newY!=actualY)){
                    comandos.emplace_back(Gcodelib::MOV, newX, newY, lineN+1);
                }
                if(newPotencia!=actualPotencia){
                    comandos.emplace_back(Gcodelib::POWER, newPotencia, lineN+1);
                }else{
                    comandos.emplace_back(Gcodelib::POWER, actualPotencia, lineN+1);
                }

            }else if(primerToken.compare("01")==0 || primerToken.compare("1")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 1"<<linea;
                buscarEnTokens(tokens, modoCoordsRelativo, actualX, actualY, newX, newY, newPotencia, newSpeed, newCentroX, newCentroY);
                if(newPotencia!=actualPotencia){
                    comandos.emplace_back(Gcodelib::POWER, newPotencia, lineN+1);
                }
                if((newX!=actualX)||(newY!=actualY)){
                    comandos.emplace_back(Gcodelib::MOV, newX, newY, lineN+1, 1000);
                }


            }else if(primerToken.compare("02")==0 || primerToken.compare("2")==0){
                //Circulo en sentido del reloj
                //qDebug()<<Q_FUNC_INFO<<"GCode 02"<<linea;
                newCentroX = actualX;
                newCentroY = actualY;
                buscarEnTokens(tokens, modoCoordsRelativo, actualX, actualY, newX, newY, newPotencia, newSpeed, newCentroX, newCentroY);
                auto distX = newX-newCentroX;
                auto distY = newY-newCentroY;
                auto radio = std::sqrt(distX*distX+distY*distY);
                auto anguloInicial = std::atan2(actualY-newCentroY, actualX-newCentroX);
                auto anguloFinal = std::atan2(newY-newCentroY, newX-newCentroX);
                //Como es en sentido del reloj, anguloInicial ha de ser mayor que anguloFinal
                if(anguloInicial<=anguloFinal){
                    anguloFinal-=2*M_PI;
                }
                auto distanciaTotal = std::abs(radio*(anguloFinal-anguloInicial));
                long nPuntos = std::ceil(distanciaTotal*2.0);
                auto pasoAngulo = (anguloFinal-anguloInicial)/nPuntos;
                auto anguloActual = anguloInicial+pasoAngulo;
                //GenerarN segmentos de línea
                for(int j = 0; j<nPuntos; j++){
                    auto xNuevo = newCentroX+radio*std::cos(anguloActual);
                    auto YNuevo = newCentroY+radio*std::sin(anguloActual);
                    comandos.emplace_back(Gcodelib::MOV, xNuevo, YNuevo, lineN+1, 10);
                    //lineN++;
                    anguloActual += pasoAngulo;
                }
                //omitIncreaselineN = true;
            }else if(primerToken.compare("03")==0 || primerToken.compare("3")==0){
                //Circulo contra el sentido del reloj
                //qDebug()<<Q_FUNC_INFO<<"GCode 02"<<linea;
                newCentroX = actualX;
                newCentroY = actualY;
                buscarEnTokens(tokens, modoCoordsRelativo, actualX, actualY, newX, newY, newPotencia, newSpeed, newCentroX, newCentroY);
                auto distX = newX-newCentroX;
                auto distY = newY-newCentroY;
                auto radio = std::sqrt(distX*distX+distY*distY);
                auto anguloInicial = std::atan2(actualY-newCentroY, actualX-newCentroX);
                auto anguloFinal = std::atan2(newY-newCentroY, newX-newCentroX);
                //Como es en contra del sentido del reloj, anguloFinal ha de ser mayor que anguloInicial
                if(anguloFinal<=anguloInicial){
                    anguloInicial-=2*M_PI;
                }
                auto distanciaTotal = std::abs(radio*(anguloFinal-anguloInicial));
                long nPuntos = std::ceil(distanciaTotal*2.0);
                auto pasoAngulo = (anguloFinal-anguloInicial)/nPuntos;
                auto anguloActual = anguloInicial+pasoAngulo;
                //GenerarN segmentos de línea
                for(int j = 0; j<nPuntos; j++){
                    auto xNuevo = newCentroX+radio*std::cos(anguloActual);
                    auto YNuevo = newCentroY+radio*std::sin(anguloActual);
                    comandos.emplace_back(Gcodelib::MOV, xNuevo, YNuevo, lineN+1, 10);
                    //lineN++;
                    anguloActual += pasoAngulo;
                }
                //omitIncreaselineN = true;
            }else if(primerToken.compare("03")==0 || primerToken.compare("3")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 03"<<linea;
            }else if(primerToken.compare("17")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 17, seleccioanr plano xy"<<linea;
            }else if(primerToken.compare("21")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 21"<<linea;
            }else if(primerToken.compare("28")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 28"<<linea;
                //HOME
                comandos.emplace_back(Gcodelib::HOME, lineN+1);
            }else if(primerToken.compare("41")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 41"<<linea;
            }else if(primerToken.compare("42")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 42"<<linea;
            }else if(primerToken.compare("54")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 54"<<linea;
            }else if(primerToken.compare("90")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 90"<<linea;
                modoCoordsRelativo = false;
            }else if(primerToken.compare("91")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 91"<<linea;
                modoCoordsRelativo = true;
            }else if(primerToken.compare("94")==0){
                qDebug()<<Q_FUNC_INFO<<"GCode 94, mm/min"<<linea;
                modoCoordsRelativo = true;
            }else{
                qDebug()<<Q_FUNC_INFO<<"Comando no interpretable:"<<linea;
                ret = -2;
                break;
            }
        }else{
            qDebug()<<Q_FUNC_INFO<<"Error, los comandos deben comenzar con G"<<linea;
            file.close();
            ret = -1;
            break;
        }

        actualX = newX;
        actualY  = newY;
        actualPotencia = newPotencia;
        actualSpeed = newSpeed;
        if(!omitIncreaselineN){
            lineN++;
        }
        omitIncreaselineN = false;
    }

    file.close();

    //Generar trayectoria
    int puntoAnteriorX = 0;
    int puntoAnteriorY = 0;
    for(auto& linea :  comandos){
        if(linea.tipo!=Gcodelib::MOV){
            comandosConTiempo.emplace_back(
                linea.tipo,
                linea.potenciaLaser,
                linea.destinoX,
                linea.destinoY,
                linea.lineaOrigen,
                linea.tiempoEspera
            );
            continue;
        }
        //Si el movimiento es de ambos ejes, limitar la aceleración máxima
        //y la velocidad
        if((linea.destinoX!=puntoAnteriorX)&&(linea.destinoY!=puntoAnteriorY)){
            accMax/=M_SQRT2;
            vMax/=M_SQRT2;
        }
        double distanciaX = (linea.destinoX-puntoAnteriorX);
        double distanciaY = (linea.destinoY-puntoAnteriorY);
        double distancia = sqrt(distanciaX*distanciaX+distanciaY*distanciaY);
        double t1Deseado = vMax/accMax;
        double t1Calculado = distancia/vMax;
        double t2Calculado = 0.0;
        bool casoFullSpeed = false;
        if(t1Calculado>=t1Deseado){
            //Sí se puede alcanzar al velocidad final
            t2Calculado = t1Calculado-t1Deseado;
            t1Calculado = t1Deseado;
            casoFullSpeed = true;
        }else{
            //No se puede alcanzar la velocidad final

        }

        if(casoFullSpeed){
            double tiempoAvance = t1Calculado*1000;
            double tiempoVConstante = t2Calculado*1000;
            double tiempoFrenado = t1Calculado*1000;
            double tiempoTotal = tiempoAvance+tiempoVConstante+tiempoFrenado;
            comandosConTiempo.emplace_back(
                Gcodelib::MOV,
                linea.potenciaLaser,
                puntoAnteriorX+(t1Calculado/tiempoTotal)*distanciaX,
                puntoAnteriorY+(t1Calculado/tiempoTotal)*distanciaY,
                linea.lineaOrigen,
                tiempoAvance
            );
            comandosConTiempo.emplace_back(
                Gcodelib::MOV,
                linea.potenciaLaser,
                puntoAnteriorX+((t1Calculado+t2Calculado)/tiempoTotal)*distanciaX,
                puntoAnteriorY+((t1Calculado+t2Calculado)/tiempoTotal)*distanciaY,
                linea.lineaOrigen,
                tiempoVConstante
            );
            comandosConTiempo.emplace_back(
                Gcodelib::MOV,
                linea.potenciaLaser,
                linea.destinoX,
                linea.destinoY,
                linea.lineaOrigen,
                tiempoFrenado
            );

        }else{
            long tiempoTotal_mitad = 1000.0*sqrt(distancia/accMax);
            comandosConTiempo.emplace_back(
                Gcodelib::MOV,
                linea.potenciaLaser,
                puntoAnteriorX+(0.5)*distanciaX,
                puntoAnteriorY+(0.5)*distanciaY,
                linea.lineaOrigen,
                tiempoTotal_mitad
            );
            comandosConTiempo.emplace_back(
                Gcodelib::MOV,
                linea.potenciaLaser,
                linea.destinoX,
                linea.destinoY,
                linea.lineaOrigen,
                tiempoTotal_mitad
            );

        }
        puntoAnteriorX= linea.destinoX;
        puntoAnteriorY= linea.destinoY;


    }

    return ret;
}
