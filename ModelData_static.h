#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shape.h"
#include "Matrix.h"
using namespace std;


class ModelData_static {

public:

    struct ModelData_struct {
        unique_ptr<Shape> shape;
        GLuint texture;
        Matrix transform;
    };

    ModelData_struct model_st;

public:
    ModelData_static();
    virtual ~ModelData_static();
    void ModelLoad(const string& objFileName, const char* TextureFileName);

private:
    //�R�s�[�C���X�g���N�^�ɂ��R�s�[�֎~
    ModelData_static(const  ModelData_static& m) = delete;
    //����ɂ��R�s�[�֎~
    ModelData_static& operator=(const ModelData_static& m) = delete;
};
