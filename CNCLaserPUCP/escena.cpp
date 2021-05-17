#include "escena.hpp"

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QForwardRenderer>
#include <QQuaternion>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <QCameraLens>
#include "orbittransformcontroller.hpp"
#include "qorbitcameracontroller.h"
#include <QMetalRoughMaterial>
#include <QEnvironmentLight>
#include <QPointLight>
#include <QResizeEvent>
#include <QGoochMaterial>
#include <QPushButton>

Escena::Escena(QWidget *parent)
    : QWidget(parent)
{
    view = new Qt3DExtras::Qt3DWindow();

    // create a container for Qt3DWindow
    container = createWindowContainer(view,this);

    // background color
    view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x575757)));

    // Root entity
    auto rootEntity = new Qt3DCore::QEntity();
    root = rootEntity;
    // Cuboid
    auto cuboidMesh = new Qt3DExtras::QCuboidMesh();
    // CuboidMesh Transform
    auto cuboidTransform = new Qt3DCore::QTransform();
    cuboidTransform->setScale3D(QVector3D(1.2f, 2.3f, 1.0f));
    cuboidTransform->setTranslation(QVector3D(0.0f, 0.0f, -1.1f));
    auto cuboidMaterial = new Qt3DExtras::QGoochMaterial();
    // assamble entity
    auto cuboidEntity = new Qt3DCore::QEntity(rootEntity);
    cuboidEntity->addComponent(cuboidMesh);
    cuboidEntity->addComponent(cuboidMaterial);
    cuboidEntity->addComponent(cuboidTransform);

    float alturaCamara = 6.0f;
    //Luz
    Qt3DCore::QEntity *luz = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(luz);
    light->setColor("white");
    light->setIntensity(1.);
    light->setLinearAttenuation(1./alturaCamara);
    light->setQuadraticAttenuation(0.1/alturaCamara);
    auto lightTransform = new Qt3DCore::QTransform();
    lightTransform->setScale(1.0f);
    lightTransform->setTranslation(QVector3D(0.0f, 0.0f, alturaCamara));
    luz->addComponent(lightTransform);
    luz->addComponent(light);


    // Camera
    //auto camera = new QCamera(rootEntity,view);
    auto cameraEntity = view->camera();
    cameraEntity->setProjectionType(Qt3DRender::QCameraLens::OrthographicProjection);
    cameraEntity->setPosition(QVector3D(0, 0, alturaCamara));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));
    cameraEntity->setTop(2.4/2);
    cameraEntity->setBottom(-2.4/2);
    cameraEntity->setLeft(-1.2/2);
    cameraEntity->setRight(1.2/2);
    cameraEntity->setNearPlane(0.01);
    cameraEntity->setFarPlane(20);

    // Set root object of the scene
    view->setRootEntity(rootEntity);

    //    //orbit controller
    //    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    //    camController->setLinearSpeed( 50.0f );
    //    camController->setLookSpeed( 180.0f );
    //    camController->setCamera(cameraEntity);


}

void
Escena::resizeView(QSize size)
{
    container->resize(size);
}

void
Escena::resizeEvent ( QResizeEvent* ev )
{
    resizeView(this->size());
    auto sz = ev->size();
    float aspectRatio = sz.width()*1.0;
    aspectRatio = aspectRatio/sz.height();
    auto cameraEntity = view->camera();
    if(aspectRatio>=1.2/2.4){
        cameraEntity->lens()->setOrthographicProjection(
            -2.4*aspectRatio/2, 2.4*aspectRatio/2,
            -2.4/2, 2.4/2,
            0.1, 20
        );
    }else{
        cameraEntity->lens()->setOrthographicProjection(
            -1.2/2,1.2/2,
            -1.2/aspectRatio/2, 1.2/aspectRatio/2,
            0.1, 20
        );
    }

}

void Escena::agregarLinea(QVector3D inicio, QVector3D fin){

}

void Escena::addTest(){
    // Cuboid
    auto cuboidMesh = new Qt3DExtras::QSphereMesh();
    // CuboidMesh Transform
    auto cuboidTransform = new Qt3DCore::QTransform();
    cuboidTransform->setScale3D(QVector3D(0.2f, 0.2f, 0.2f));
    cuboidTransform->setTranslation(QVector3D(0.0f, 0.0f, 0.0f));
    auto cuboidMaterial = new Qt3DExtras::QPhongMaterial();
    // assamble entity
    auto cuboidEntity = new Qt3DCore::QEntity(root);
    cuboidEntity->addComponent(cuboidMesh);
    cuboidEntity->addComponent(cuboidMaterial);
    cuboidEntity->addComponent(cuboidTransform);
}
