#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QOpenGLWidget>
#include <QGridLayout>
#include "escena.hpp"

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
        ui->button, &QPushButton::clicked,
        escena, &Escena::addTest
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

