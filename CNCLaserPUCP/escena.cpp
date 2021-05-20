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
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#include <QCameraLens>
#include "orbittransformcontroller.hpp"
#include "qorbitcameracontroller.h"
#include <QMetalRoughMaterial>
#include <QEnvironmentLight>
#include <QPointLight>
#include <QResizeEvent>
#include <QGoochMaterial>
#include <QPushButton>

constexpr float alturaCamara = 6.0f;

void Escena::recargarVista(){
    if(camera==nullptr){
        return;
    }
    camera->setPosition(QVector3D(0, 0, alturaCamara));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setViewCenter(QVector3D(0, 0, 0));
}

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
    cuboidTransform->setScale3D(QVector3D(1.2f, 2.4f, 1.0f));
    cuboidTransform->setTranslation(QVector3D(0.0f, 0.0f, -1.1f));
    auto cuboidMaterial = new Qt3DExtras::QGoochMaterial();
    // assamble entity
    auto cuboidEntity = new Qt3DCore::QEntity(rootEntity);
    cuboidEntity->addComponent(cuboidMesh);
    cuboidEntity->addComponent(cuboidMaterial);
    cuboidEntity->addComponent(cuboidTransform);


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
    camera = cameraEntity;
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

    //orbit controller
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(cameraEntity);


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

void Escena::agregarLinea(QVector3D inicio, QVector3D fin, int laserPower){
    //definir geometría
    auto* geometry = new Qt3DRender::QGeometry(root);
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(float));
    float *positions = reinterpret_cast<float*>(bufferBytes.data());
    *positions++ = inicio.x()-1.2/2.0f;
    *positions++ = inicio.y()-2.4/2.0f;
    *positions++ = inicio.z();
    *positions++ = fin.x()-1.2/2.0f;
    *positions++ = fin.y()-2.4/2.0f;
    *positions++ = fin.z();
    auto *buf = new Qt3DRender::QBuffer(geometry);
    buf->setData(bufferBytes);
    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    geometry->addAttribute(positionAttribute);
    //conectar vértices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;
    auto *indexBuffer = new Qt3DRender::QBuffer(geometry);
    indexBuffer->setData(indexBytes);
    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute);
    auto *line = new Qt3DRender::QGeometryRenderer(root);
    //mesh
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(root);
    if(laserPower==0){
        material->setAmbient(QRgb(0x00FF00));
    }else{
        material->setAmbient(QRgb(0xFF0000));
    }

    //entidad
    auto *lineEntity = new Qt3DCore::QEntity(root);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);
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
