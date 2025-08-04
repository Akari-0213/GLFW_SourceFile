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


class ModelData {

public:

    struct ModelData_struct {
        unique_ptr<Shape> shape;
        GLuint texture;
        Matrix transform;
    };

    ModelData_struct model_st;


public:
    ModelData();
    virtual ~ModelData();
    void ModelLoad(const string& objFileName, const char* TextureFileName);

private:
    //コピーインストラクタによるコピー禁止
    ModelData(const  ModelData& m) = delete;
    //代入によるコピー禁止
    ModelData& operator=(const ModelData& m) = delete;
};
