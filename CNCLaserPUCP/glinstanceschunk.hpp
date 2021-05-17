#ifndef GLCHUNK_HPP
#define GLCHUNK_HPP
#include <qglobal.h>

#ifdef Q_OS_WIN
#include "glutils_win.h"
#endif //Q_OS_WIN
#include <array>
#include <QOpenGLFunctions>
#include <vector>
#include <QOpenGLExtraFunctions>
#ifdef QT_DEBUG
#include <iostream>
#endif //QT_DEBUG

class QOpenGLShaderProgram;
template<
    int vertex_attr0_NElementos_param = 3,//por ejemplo, posicion
    int vertex_attr1_NElementos_param = 2,//por ejemplo, uv
    int inst_attr0_NElementos_param = 0,//por ejemplo, size
    int inst_attr1_NElementos_param =0,//por ejemplo, posicion,
    typename IndexesType = GLushort
>
struct GLInstancesChunk{
    bool finishedQ = true;
    QString nombre;
    #ifdef QT_DEBUG

    #endif //QT_DEBUG
    GLInstancesChunk(const QString& nombre){
        this->nombre = nombre;
        finishedQ = true;
        #ifdef QT_DEBUG
        std::cout<<"GLInstancesChunk creada"<<std::endl;
        #endif //QT_DEBUG
    }
    ~GLInstancesChunk(){
        #ifdef QT_DEBUG
        std::cout<<"GLInstancesChunk destruida, finished: "<< finishedQ<<std::endl;
        if(!finishedQ){
            std::cout<<"GLInstancesChunk destruida sin invocar finishGL"<<std::endl;
        }
        #endif //QT_DEBUG
    }
    static constexpr int  vertex_attr0_NElementos = vertex_attr0_NElementos_param;
    static constexpr int  vertex_attr1_NElementos = vertex_attr1_NElementos_param;
    static constexpr int  vertex_NElementos = (vertex_attr0_NElementos+vertex_attr1_NElementos);
    static constexpr int  vertex_attr_stride = sizeof( GLfloat ) * ( vertex_NElementos );

    static constexpr int inst_attr0_NElementos = inst_attr0_NElementos_param;
    static constexpr int inst_attr1_NElementos = inst_attr1_NElementos_param;
    static constexpr int instance_NElements = (inst_attr0_NElementos+inst_attr1_NElementos);
    static constexpr int inst_attr_stride = sizeof( GLfloat ) * ( instance_NElements );

    static constexpr int  VERTEX_ATTR0_INDX = 0;
    static constexpr int  VERTEX_ATTR1_INDX = 1;
    static constexpr int  INSTANCE_ATTR0_INDX = 2;
    static constexpr int  INSTANCE_ATTR1_INDX = 3;

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> attributosInstancias;
    std::vector<IndexesType> indices;

    std::vector<GLuint> indexesVBOs;

    static constexpr bool hayDatosInstanciasQ = (instance_NElements>0);
    static constexpr int nUsedVBOs = (hayDatosInstanciasQ?2:1);
    GLuint objecto_VBOArray;
    std::array<GLuint, nUsedVBOs > objecto_VBOs;//vertices e instancias

    void addIndexesVBOs(int count, QOpenGLExtraFunctions& gl){
        if(count <= 0){
            return;
        }
        int orgSize = indexesVBOs.size();
        indexesVBOs.resize(orgSize+count);
        gl.glGenBuffers(count, &indexesVBOs[orgSize]);

    }

    void removeLastVBOs(int count, QOpenGLExtraFunctions& gl){
        int orgSize = indexesVBOs.size();
        if(count > orgSize){
            return;
        }
        gl.glDeleteBuffers(count, &indexesVBOs[orgSize-count]);
        indexesVBOs.resize(orgSize-count);

    }

    void updateIndexesData(int start, int len, int vboIndex, QOpenGLExtraFunctions& gl){
        if(start<0){
            return;
        }
        if( (start+len)>indices.size() ){
            return;
        }
        gl.glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, indexesVBOs[vboIndex]);
        gl.glBufferData ( GL_ELEMENT_ARRAY_BUFFER, len*sizeof(IndexesType),
                         &indices[start], GL_DYNAMIC_DRAW );
    }

    GLfloat* attributesForInstance(int instance){
        return &attributosInstancias[instance*instance_NElements];
    }

    void initGL(QOpenGLExtraFunctions& gl){
        finishedQ = false;
        //std::array<GLushort, 4> indices = { 0, 1, 2, 3 };
        gl.glGenVertexArrays (1, &objecto_VBOArray);
        gl.glBindVertexArray (objecto_VBOArray);
        gl.glGenBuffers ( objecto_VBOs.size(), objecto_VBOs.data() );


        updateVertexData(gl);
        gl.glEnableVertexAttribArray ( VERTEX_ATTR0_INDX );
        if(vertex_attr1_NElementos_param>0){
            gl.glEnableVertexAttribArray ( VERTEX_ATTR1_INDX );
        }

        if(hayDatosInstanciasQ){
            updateInstanceData(gl);

        }
        if(inst_attr0_NElementos>0){
            gl.glEnableVertexAttribArray(INSTANCE_ATTR0_INDX);
        }
        if(inst_attr1_NElementos>0){
            gl.glEnableVertexAttribArray(INSTANCE_ATTR1_INDX);
        }

        #ifdef QT_DEBUG
        auto glErroCode = gl.glGetError();
        if(glErroCode!=GL_NO_ERROR){
            qDebug("error opengl");
        }
        #endif //QT_DEBUG

    }
    void bindVertexData(QOpenGLExtraFunctions& gl){
        gl.glBindBuffer ( GL_ARRAY_BUFFER, objecto_VBOs[0] );
    }
    void updateVertexData(QOpenGLExtraFunctions& gl){
        gl.glBindBuffer ( GL_ARRAY_BUFFER, objecto_VBOs[0] );
        gl.glBufferData ( GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat),  vertices.data(), GL_DYNAMIC_DRAW );

    }
    void setVertexPointer(QOpenGLExtraFunctions& gl, int offset){
        gl.glVertexAttribPointer ( VERTEX_ATTR0_INDX, vertex_attr0_NElementos,
                                   GL_FLOAT, GL_FALSE, vertex_attr_stride,
                                   (const void*)offset
        );
        gl.glVertexAttribDivisor(VERTEX_ATTR0_INDX, 0);
        if(vertex_attr1_NElementos>0){
            offset+=vertex_attr0_NElementos*sizeof(GLfloat);
            gl.glVertexAttribPointer ( VERTEX_ATTR1_INDX, vertex_attr1_NElementos,
                                       GL_FLOAT, GL_FALSE, vertex_attr_stride,
                                       (const void*)offset
            );
            gl.glVertexAttribDivisor(VERTEX_ATTR1_INDX, 0);
        }
    }

    void updateInstanceData(QOpenGLExtraFunctions& gl){
        if(hayDatosInstanciasQ){
            gl.glBindBuffer (GL_ARRAY_BUFFER, objecto_VBOs[1]);
            gl.glBufferData (
                GL_ARRAY_BUFFER, attributosInstancias.size()*sizeof(GLfloat),
                attributosInstancias.data(), GL_DYNAMIC_DRAW
            );
        }
    }

    void setInstancePointer(QOpenGLExtraFunctions& gl, int offset){
        if(inst_attr0_NElementos>0){
            gl.glVertexAttribPointer ( INSTANCE_ATTR0_INDX, inst_attr0_NElementos,
                                       GL_FLOAT, GL_FALSE, inst_attr_stride,
                                       ( const void * ) offset
            );
            gl.glVertexAttribDivisor(INSTANCE_ATTR0_INDX, 1);
        }
        if(inst_attr1_NElementos>0){
            offset+=inst_attr0_NElementos*sizeof(GLfloat);
            gl.glVertexAttribPointer ( INSTANCE_ATTR1_INDX, inst_attr1_NElementos,
                                       GL_FLOAT, GL_FALSE, inst_attr_stride,
                                       ( const void * ) offset
            );
            gl.glVertexAttribDivisor(INSTANCE_ATTR1_INDX, 1);
        }
    }

    void finishGL(QOpenGLExtraFunctions& gl){
        gl.glDeleteBuffers(objecto_VBOs.size(), objecto_VBOs.data() );
        gl.glDeleteVertexArrays(1, &objecto_VBOArray);
        for(auto indexVBO:indexesVBOs){
            gl.glDeleteBuffers(1, &indexVBO);
        }
        indexesVBOs.clear();
        finishedQ = true;
    }

    void clearRAM(){
        vertices.clear();
        attributosInstancias.clear();
        indices.clear();
    }

    int verticesAttr0Offset(int vertex)const{
        return (vertex*vertex_NElementos);
    }
    int verticesAttr1Offset(int vertex)const{
        return (vertex*vertex_NElementos+vertex_attr0_NElementos);
    }
    int instancesAttr0Offset(int instance)const{
        return (instance*instance_NElements);
    }
    int instancesAttr1Offset(int instance)const{
        return (instance*instance_NElements+inst_attr0_NElementos);
    }
    int indexesOffset(int instancesOffset)const{
        return instancesOffset;
    }
    void reserveForAtLeastNVertexs(int vertexN){
        int expC = vertexN*vertex_NElementos;
        if(vertices.size()<expC){
            vertices.resize(expC);
        }
    }

    void reserveForAtLeastNInstances(int instancesN){
        int expC = instancesN*instance_NElements;
        if(attributosInstancias.size()<expC){
            attributosInstancias.resize(expC);
        }
    }

    void reserveForAtLeastNIndexes(int indexesN){
        if(indices.size()<indexesN){
            indices.resize(indexesN);
        }
    }
};

typedef  struct {
    uint  count = 0;
    uint  instanceCount = 1;
    uint  firstIndex = 0;
    int   baseVertex = 0;
    uint  reservedMustBeZero = 0;
} DrawElementsIndirectCommand;


#endif // GLCHUNK_HPP
