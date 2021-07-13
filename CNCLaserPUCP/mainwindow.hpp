#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "gcodelib.hpp"
#include <QVector3D>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class QOpenGLWidget;
class Escena;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Escena* escena = nullptr;
    void agregarMensaje(const QString& msj);
    std::vector<Gcodelib::Comando> cmds;
    std::vector<Gcodelib::Comando> cmdsTrayectoria;
public slots:
    void cargarArchivo();

signals:
    void agregarLinea(QVector3D inicio, QVector3D fin, int laserPower);


};
#endif // MAINWINDOW_HPP
