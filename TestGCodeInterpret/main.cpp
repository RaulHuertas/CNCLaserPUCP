#include <QCoreApplication>
#include <QDebug>
#include "gcodelib.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::vector<Gcodelib::Comando> cmds;
    std::vector<Gcodelib::Comando> cmdsLineas;
    int lineasLeidas = 0;
    auto ret = Gcodelib::cargarArchivo("Muestra.gcode", cmds, cmdsLineas, lineasLeidas);
    qDebug()<<"CMDS len: "<<cmds.size();

    //return a.exec();
}

