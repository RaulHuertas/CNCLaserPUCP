#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <QVector3D>

class Escena
      : public QWidget
{
    Q_OBJECT

private:
    QWidget *container;

public:
    explicit Escena(QWidget *parent = nullptr);

protected:
    // reimplementation needed to handle resize events
    // http://doc.qt.io/qt-5/qwidget.html#resizeEvent
    void
    resizeEvent ( QResizeEvent * event );

public slots:
    void resizeView(QSize size);
    void addTest();
    void agregarLinea(QVector3D inicio, QVector3D fin, int laserPower);
private:
    Qt3DExtras::Qt3DWindow* view = nullptr;
    Qt3DCore::QEntity* root = nullptr;
};

#endif // SCENE_H
