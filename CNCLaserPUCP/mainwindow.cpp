#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QOpenGLWidget>
#include <QGridLayout>
#include <QFileDialog>
#include <QPushButton>
#include "escena.hpp"
#include "gcodelib.hpp"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    escena = new Escena(this);
    auto ly = new QGridLayout;
    ly->addWidget(escena);
    ui->escena->setLayout(ly);
    connect(
        ui->botonCargarArchivo, &QPushButton::clicked,
        this, &MainWindow::cargarArchivo
    );
    connect(
        this, &MainWindow::agregarLinea,
        escena, &Escena::agregarLinea
    );
    connect(
        ui->botonRecargarVista, &QPushButton::clicked,
        escena, &Escena::recargarVista
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::agregarMensaje(const QString& msj){
    ui->reporte->moveCursor(QTextCursor::End);
    ui->reporte->insertPlainText(msj+"\n");
    ui->reporte->moveCursor(QTextCursor::End);
}

void MainWindow::cargarArchivo(){
    auto filename = QFileDialog::getOpenFileName(
        this,
        tr("Cargar instrucciones"), "./", tr("Archivos (*.gcode *.cnc *.g *.nc)")
    );
    if(filename.size()==0){
        return;
    }
    agregarMensaje("Cargando archivo: "+filename);
    cmds.clear();
    cmds.reserve(1000);
    int lineasLeidas = 0;
    auto ret = Gcodelib::cargarArchivo(filename, cmds, lineasLeidas);
    if(cmds.size()>0){
        agregarMensaje(
            QString::number(lineasLeidas)+" líneas leídas"
        );
        agregarMensaje(
            QString::number(cmds.size())+" comandos creados"
        );
        float oldX = 0;
        float oldY = 0;
        int oldLaser = 0;
        float escala = 10000.0f;
        for(const auto& cmd : cmds){
            if(cmd.isMovQ()){
                emit agregarLinea(
                    QVector3D(oldX/escala, oldY/escala, 0.0f),
                    QVector3D(cmd.destinoX/escala, cmd.destinoY/escala, 0.0f),
                    oldLaser
                );
                oldX = cmd.destinoX;
                oldY = cmd.destinoY;
            }else if(cmd.isLaserQ()){
                oldLaser = cmd.potenciaLaser;
            }
        }
    }else{
        agregarMensaje("NO FUE POSIBLE INTERPRETAR EL ARCHIVO");
    }
}
