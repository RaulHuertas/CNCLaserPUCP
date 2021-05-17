#include <QCoreApplication>
#include "gcodelib.hpp"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::vector<Gcodelib::Comando> cmds;
    auto ret = Gcodelib::cargarArchivo("Muestra.gcode", cmds);
    qDebug()<<"CMDS len: "<<cmds.size();

    //return a.exec();
}

