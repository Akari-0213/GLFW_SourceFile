#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shape.h"
#include "SolidShapeIndex.h"
#include "Matrix.h"
#include "Load_ObjFile.h"
#include "Load_Texture.h"
#include "ModelData_static.h"
using namespace std;



ModelData_static::ModelData_static() {
    model_st.texture = 0;
    model_st.transform = Matrix::identity();
}

ModelData_static::~ModelData_static() {
    glDeleteTextures(1, &model_st.texture);
}

void ModelData_static::ModelLoad(const string& objFileName, const char* TextureFileName)
{


    Load_ObjFile obj;
    obj.FileScan(objFileName);
    if (obj.vertices.empty()) {
        cout << "objƒtƒ@ƒCƒ‹“Ç‚Ýž‚ÝŽ¸”s" << endl;
    }

    vector<Object::Vertex> ObjVertices;
    vector<GLuint> ObjIndices;
    map<tuple<int, int, int>, GLuint> VertexMap;
    for (const auto& face : obj.obj_faces) {
        if (face.size() < 3) continue;
        for (size_t i = 1; i < face.size() - 1; ++i) {
            array<size_t, 3> triangleIndices = { 0, i, i + 1 };
            for (size_t j = 0; j < 3; ++j) {
                const FaceVertex& fv = face[triangleIndices[j]];

                auto key = make_tuple(fv.vertex_index, fv.texCoord_index, fv.normal_index);
                auto it = VertexMap.find(key);
                if (it != VertexMap.end()) {
                    ObjIndices.push_back(it->second);
                }
                else {
                    if (fv.vertex_index < 0 || fv.vertex_index >= obj.vertices.size()) {
                        cout << "Invalid vertex index" << endl;
                        continue;
                    }
                    glm::vec3 pos = obj.vertices[fv.vertex_index].vertex_position;

                    glm::vec3 normal(0.0f, 1.0f, 0.0f);
                    if (fv.normal_index >= 0 && fv.normal_index < obj.normals.size())
                    {
                        normal = obj.normals[fv.normal_index].normal_vector;
                    }


                    glm::vec2 texcoord(0.0f, 0.0f);
                    if (fv.texCoord_index >= 0 && fv.texCoord_index < obj.texcoords.size())
                    {
                        texcoord = obj.texcoords[fv.texCoord_index].texcoord;
                    }

                    Object::Vertex vertex = {
                        pos.x, pos.y, pos.z,
                        normal.x, normal.y, normal.z,
                        texcoord.x, texcoord.y
                    };

                    GLuint newIndex = static_cast<GLuint>(ObjVertices.size());
                    ObjVertices.push_back(vertex);
                    ObjIndices.push_back(newIndex);
                    VertexMap[key] = newIndex;
                }
            }
        }
    }

    model_st.texture = Load_PNG_Texture(TextureFileName);
    if (model_st.texture == 0) {
        cout << "Faild to load Texture" << endl;
        return;
    }

    model_st.shape.reset(new SolidShapeIndex(3, static_cast<GLsizei>(ObjVertices.size()), ObjVertices.data(), static_cast<GLsizei>(ObjIndices.size()), ObjIndices.data()));
    model_st.transform = Matrix();
}