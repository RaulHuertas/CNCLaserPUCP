#include "gcodelib.hpp"
#include <QFile>
#include <QDebug>

Gcodelib::Gcodelib()
{

}

void buscarEnTokens(
    QStringList tokens,
    bool coordRelativeQ,
    int oldX,
    int oldY,
    int& newX,
    int& newY,
    int& newPower,
    int& newSpeed
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
            newX = (resto.toDouble()*10);
            if(coordRelativeQ){
                newX+=oldX;
            }
        }else if(cmd=='Y'){
            newY = (resto.toDouble()*10);
            if(coordRelativeQ){
                newY+=oldY;
            }
        }else if(cmd=='S'){//potencia laser
            newPower = resto.toDouble();
        }else if(cmd=='F'){//velocidad de desplazamiento
            newSpeed = resto.toDouble();
        }
    }
}

int Gcodelib::cargarArchivo(
    const QString& filename,
    std::vector<Comando>& comandos
){
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return 1;
    }
    int lineN = 0;
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
        if(!linea.startsWith("G")){
            qDebug()<<Q_FUNC_INFO<<"Error, los comandos deben comenzar con G"<<linea;
            file.close();
            ret = -1;
            break;
        }
        qDebug()<<Q_FUNC_INFO<<"EVA: "<<linea;
        auto tokens = linea.split(" ");
        auto primerToken = tokens[0];
        primerToken = primerToken.mid(1);
        int comando = 0;
        if(primerToken.compare("00")==0){
            qDebug()<<Q_FUNC_INFO<<"GCode 0"<<linea;
            buscarEnTokens(tokens, modoCoordsRelativo, actualX, actualY, newX, newY, newPotencia, newSpeed);
            if(newPotencia!=actualPotencia){
                comandos.emplace_back(Gcodelib::POWER, newPotencia, lineN+1);
            }
            if((newX!=actualX)||(newY!=actualY)){
                comandos.emplace_back(Gcodelib::MOV, newX, newY, lineN+1);
            }

        }else if(primerToken.compare("01")==0){
            qDebug()<<Q_FUNC_INFO<<"GCode 1"<<linea;
        }else if(primerToken.compare("02")==0){
            qDebug()<<Q_FUNC_INFO<<"GCode 2"<<linea;
        }else if(primerToken.compare("03")==0){
            qDebug()<<Q_FUNC_INFO<<"GCode 3"<<linea;
        }else if(primerToken.compare("21")==0){
            qDebug()<<Q_FUNC_INFO<<"GCode 21"<<linea;
        }else if(primerToken.compare("28")==0){
            qDebug()<<Q_FUNC_INFO<<"GCode 28"<<linea;
            //HOME
            comandos.emplace_back(Gcodelib::HOME, lineN+1);
        }else if(primerToken.compare("90")==0){
            qDebug()<<Q_FUNC_INFO<<"GCode 90"<<linea;
            modoCoordsRelativo = false;
        }else if(primerToken.compare("91")==0){
            qDebug()<<Q_FUNC_INFO<<"GCode 91"<<linea;
            modoCoordsRelativo = true;
        }else if(primerToken.compare("41")==0){
            qDebug()<<Q_FUNC_INFO<<"GCode 41"<<linea;
        }else if(primerToken.compare("42")==0){
            qDebug()<<Q_FUNC_INFO<<"GCode 42"<<linea;
        }else{
            qDebug()<<Q_FUNC_INFO<<"Comando no interpretable:"<<linea;
            ret = -2;
            break;
        }
        actualX = newX;
        actualY  = newY;
        actualPotencia = newPotencia;
        actualSpeed = newSpeed;
        lineN++;
    }

    file.close();
    return ret;
}
