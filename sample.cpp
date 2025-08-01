#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <map>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Matrix.h"
#include "Vector_light.h"
#include "Shape.h"
#include "ShapeIndex.h"
#include "SolidShape.h"
#include "SolidShapeIndex.h"
#include "Uniform.h"
#include "Material.h"
#include "Load_ObjFile.cpp"

// �V�F�[�_�I�u�W�F�N�g�̃R���p�C�����ʂ�\������
//   shader: �V�F�[�_�I�u�W�F�N�g��
//   str: �R���p�C���G���[�����������ꏊ������������
GLboolean printShaderInfoLog(GLuint shader, const char* str)
{
    // �R���p�C�����ʂ��擾����
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;

    // �V�F�[�_�̃R���p�C�����̃��O�̒������擾����
    GLsizei bufSize;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

    if (bufSize > 1)
    {
        // �V�F�[�_�̃R���p�C�����̃��O�̓��e���擾����
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }

    return static_cast<GLboolean>(status);
}

// �v���O�����I�u�W�F�N�g�̃����N���ʂ�\������
//   program: �v���O�����I�u�W�F�N�g��
GLboolean printProgramInfoLog(GLuint program)
{
    // �����N���ʂ��擾����
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Link Error." << std::endl;

    // �V�F�[�_�̃����N���̃��O�̒������擾����
    GLsizei bufSize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

    if (bufSize > 1)
    {
        // �V�F�[�_�̃����N���̃��O�̓��e���擾����
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }

    return static_cast<GLboolean>(status);
}

// �v���O�����I�u�W�F�N�g���쐬����
//   vsrc: �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O�����̕�����
//   fsrc: �t���O�����g�V�F�[�_�̃\�[�X�v���O�����̕�����
GLuint createProgram(const char* vsrc, const char* fsrc)
{
    // ��̃v���O�����I�u�W�F�N�g���쐬����
    const GLuint program(glCreateProgram());

    if (vsrc != NULL)
    {
        // �o�[�e�b�N�X�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���쐬����
        const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
        glShaderSource(vobj, 1, &vsrc, NULL);
        glCompileShader(vobj);

        // �o�[�e�b�N�X�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���v���O�����I�u�W�F�N�g�ɑg�ݍ���
        if (printShaderInfoLog(vobj, "vertex shader"))
            glAttachShader(program, vobj);
        glDeleteShader(vobj);
    }

    if (fsrc != NULL)
    {
        // �t���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���쐬����
        const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
        glShaderSource(fobj, 1, &fsrc, NULL);
        glCompileShader(fobj);

        // �t���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���v���O�����I�u�W�F�N�g�ɑg�ݍ���
        if (printShaderInfoLog(fobj, "fragment shader"))
            glAttachShader(program, fobj);
        glDeleteShader(fobj);
    }

    // �v���O�����I�u�W�F�N�g�������N����
    glBindAttribLocation(program, 0, "position");
    glBindAttribLocation(program, 1, "normal");
    glBindFragDataLocation(program, 0, "fragment");
    glLinkProgram(program);

    // �쐬�����v���O�����I�u�W�F�N�g��Ԃ�
    if (printProgramInfoLog(program))
        return program;

    // �v���O�����I�u�W�F�N�g���쐬�ł��Ȃ���� 0 ��Ԃ�
    glDeleteProgram(program);
    return 0;
}

// �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���
//   name: �V�F�[�_�̃\�[�X�t�@�C����
//   buffer: �ǂݍ��񂾃\�[�X�t�@�C���̃e�L�X�g
bool readShaderSource(const char* name, std::vector<GLchar>& buffer)
{
    // �t�@�C������ NULL ������
    if (name == NULL) return false;

    // �\�[�X�t�@�C�����J��
    std::ifstream file(name, std::ios::binary);
    if (file.fail())
    {
        // �J���Ȃ�����
        std::cerr << "Error: Can't open source file: " << name << std::endl;
        return false;
    }

    // �t�@�C���̖����Ɉړ������݈ʒu�i���t�@�C���T�C�Y�j�𓾂�
    file.seekg(0L, std::ios::end);
    GLsizei length = static_cast<GLsizei>(file.tellg());

    // �t�@�C���T�C�Y�̃��������m��
    buffer.resize(length + 1);

    // �t�@�C����擪����ǂݍ���
    file.seekg(0L, std::ios::beg);
    file.read(buffer.data(), length);
    buffer[length] = '\0';

    if (file.fail())
    {
        // ���܂��ǂݍ��߂Ȃ�����
        std::cerr << "Error: Could not read souce file: " << name << std::endl;
        file.close();
        return false;
    }

    // �ǂݍ��ݐ���
    file.close();
    return true;
}

// �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���Ńv���O�����I�u�W�F�N�g���쐬����
//   vert: �o�[�e�b�N�X�V�F�[�_�̃\�[�X�t�@�C����
//   frag: �t���O�����g�V�F�[�_�̃\�[�X�t�@�C����
GLuint loadProgram(const char* vert, const char* frag)
{
    // �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���
    std::vector<GLchar> vsrc;
    const bool vstat(readShaderSource(vert, vsrc));
    std::vector<GLchar> fsrc;
    const bool fstat(readShaderSource(frag, fsrc));

    // �v���O�����I�u�W�F�N�g���쐬����
    return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}

// ��`�̒��_�̈ʒu
constexpr Object::Vertex rectangleVertex[] =
{
  { -0.5f, -0.5f },
  {  0.5f, -0.5f },
  {  0.5f,  0.5f },
  { -0.5f,  0.5f }
};

constexpr Object::Vertex octanhedronVertex[] =
{
  {  0.0f,  1.0f,  0.0f },
  { -1.0f,  0.0f,  0.0f },
  {  0.0f,  -1.0f,  0.0f },
  {  1.0f,  0.0f,  0.0f },
  {  0.0f,  1.0f,  0.0f },
  {  0.0f,  0.0f,  1.0f },
  {  0.0f,  -1.0f,  0.0f },
  {  0.0f,  0.0f,  -1.0f },
  {  -1.0f,  0.0f,  0.0f },
  {  0.0f,  0.0f,  1.0f },
  {  1.0f,  0.0f,  0.0f },
  {  0.0f,  0.0f,  -1.0f }
};

constexpr Object::Vertex cubeVertex[] =
{
  { -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f },  // (0) 
  { -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.8f },  // (1) 
  { -1.0f,  1.0f,  1.0f,  0.0f,  0.8f,  0.0f },  // (2) 
  { -1.0f,  1.0f, -1.0f,  0.0f,  0.8f,  0.8f },  // (3) 
  {  1.0f,  1.0f, -1.0f,  0.8f,  0.0f,  0.0f },  // (4) 
  {  1.0f, -1.0f, -1.0f,  0.8f,  0.0f,  0.8f },  // (5) 
  {  1.0f, -1.0f,  1.0f,  0.8f,  0.8f,  0.0f },  // (6) 
  {  1.0f,  1.0f,  1.0f,  0.8f,  0.8f,  0.8f }   // (7) 
};

// �ʂ��Ƃɖ@����ς����Z�ʑ̂̒��_���� 
constexpr Object::Vertex solidCubeVertex[] =
{
    // �� 
   { -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f },
   { -1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f },
   { -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f },
   { -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f },
   { -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f },
   { -1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f },

    // �� 
    {  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f },
    { -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f },

    // �� 
    { -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f },
    {  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f },
    {  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f },
    { -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f },
    {  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f },
    { -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f },

    // �E 
    {  1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f },
    {  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f },
    {  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f },
    {  1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f },
    {  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f },
    {  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f },

     // �� 
    { -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f },
    { -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f },
    {  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f },
    { -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f },
    {  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f },
    {  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f },

    // �O 
    { -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f },
    {  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f },
    { -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f },
    { -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f }
};


// �Z�ʑ̗̂Ő��̗��[�_�̃C���f�b�N�X 
constexpr GLuint wireCubeIndex[] =
{
  1, 0, // (a) 
  2, 7, // (b) 
  3, 0, // (c) 
  4, 7, // (d) 
  5, 0, // (e) 
  6, 7, // (f) 
  1, 2, // (g) 
  2, 3, // (h) 
  3, 4, // (i) 
  4, 5, // (j) 
  5, 6, // (k) 
  6, 1  // (l) 
};

// �Z�ʑ̖̂ʂ�h��Ԃ��O�p�`�̒��_�̃C���f�b�N�X 
constexpr GLuint solidCubeIndex[] =
{
    0,  1,  2,  3,  4,  5, // �� 
    6,  7,  8,  9, 10, 11, // �� 
    12, 13, 14, 15, 16, 17, // �� 
    18, 19, 20, 21, 22, 23, // �E 
    24, 25, 26, 27, 28, 29, // �� 
    30, 31, 32, 33, 34, 35  // �O 
};


int main()
{
    // GLFW ������������
    if (glfwInit() == GL_FALSE)
    {
        // �������Ɏ��s����
        std::cerr << "Can't initialize GLFW" << std::endl;
        return 1;
    }

    // �v���O�����I�����̏�����o�^����
    atexit(glfwTerminate);

    // OpenGL Version 3.2 Core Profile ��I������
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // �E�B���h�E���쐬����
    Window window;

    //�I�u�W�F�N�g���[�h
    Load_ObjFile obj;
    string file_name = "chiikawa.obj";
    obj.FileScan(file_name);

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

                    Object::Vertex vertex = {
                        pos.x, pos.y, pos.z,
                        normal.x, normal.y, normal.z
                    };

                    GLuint newIndex = static_cast<GLuint>(ObjVertices.size());
                    ObjVertices.push_back(vertex);
                    ObjIndices.push_back(newIndex);
                    VertexMap[key] = newIndex;
                }
            }
        }
    }

    // �w�i�F���w�肷��
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    //�w�ʃJ�����O��L���ɂ���
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // �f�v�X�o�b�t�@��L���ɂ��� 
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    // �v���O�����I�u�W�F�N�g���쐬����
    const GLuint program(loadProgram("point.vert", "point.frag"));

    // uniform �ϐ��̏ꏊ���擾����
    const GLint modelviewLoc(glGetUniformLocation(program, "modelview"));
    const GLint projectionLoc(glGetUniformLocation(program, "projection"));
    const GLint normalMatrixLoc(glGetUniformLocation(program, "normalMatrix"));
    const GLint LposLoc(glGetUniformLocation(program, "Lpos"));
    const GLint LambLoc(glGetUniformLocation(program, "Lamb"));
    const GLint LdiffLoc(glGetUniformLocation(program, "Ldiff"));
    const GLint LspecLoc(glGetUniformLocation(program, "Lspec"));

    // uniform block �̏ꏊ���擾����
    const GLint materialLoc(glGetUniformBlockIndex(program, "Material"));

    // uniform block �̏ꏊ�� 0 �Ԃ̌����|�C���g�Ɍ��т��� 
    glUniformBlockBinding(program, materialLoc, 0);

    //���̕�����
    const int slices(16), stacks(8);

    ////���_�̑��������
    //std::vector<Object::Vertex> solidSphereVertex;
    //for (int j = 0; j <= stacks; ++j)
    //{
    //    const float t(static_cast<float>(j) / static_cast<float>(stacks));
    //    const float y(cos(3.141593f * t)), r(sin(3.141593f * t));

    //    for (int i = 0; i <= slices; ++i)
    //    {
    //        const float s(static_cast<float>(i) / static_cast<float>(slices));
    //        const float z(r * cos(6.283185f * s)), x(r * sin(6.283185f * s));

    //        // ���_���� 
    //        const Object::Vertex v = { x, y, z, x, y, z };
    //        // ���_������ǉ����� 
    //        solidSphereVertex.emplace_back(v);
    //    }
    //}


    ////�C���f�b�N�X�����
    //std::vector<GLuint> solidSphereIndex;
    //for (int j = 0; j <= stacks; ++j)
    //{
    //    const int k((slices + 1) * j);
    //    for (int i = 0; i <= slices; ++i)
    //    {
    //        const GLuint k0(k + i);
    //        const GLuint k1(k0 + 1);
    //        const GLuint k2(k1 + slices);
    //        const GLuint k3(k2 + 1);

    //        // �����̎O�p�` 
    //        solidSphereIndex.emplace_back(k0);
    //        solidSphereIndex.emplace_back(k2);
    //        solidSphereIndex.emplace_back(k3);
    //        // �E��̎O�p�` 
    //        solidSphereIndex.emplace_back(k0);
    //        solidSphereIndex.emplace_back(k3);
    //        solidSphereIndex.emplace_back(k1);
    //    }
    //}

    // �}�`�f�[�^���쐬����
    std::unique_ptr<const Shape> shape(new SolidShapeIndex(3, static_cast<GLsizei>(ObjVertices.size()), ObjVertices.data(), static_cast<GLsizei>(ObjIndices.size()), ObjIndices.data()));

    //�����f�[�^
    static constexpr int Lcount(2);
    static constexpr Vector_light Lpos[] = { 0.0f, 0.0f, 5.0f, 1.0f, 10.0f, 0.0f, 0.0f, 1.0f };
    static constexpr GLfloat Lamb[] = { 0.5f, 0.5f, 0.5f, 0.1f, 0.1f, 0.1f };
    static constexpr GLfloat Ldiff[] = { 1.0f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f };
    static constexpr GLfloat Lspec[] = { 1.0f, 0.5f, 0.5f,  0.9f, 0.9f, 0.9f };

    static constexpr Material color[] =
    {
        { 0.5f, 0.5f, 0.5f,  0.6f, 0.6f, 0.2f,  0.3f, 0.3f, 0.3f,  30.0f },
        { 0.5f, 0.5f, 0.5f,  0.1f, 0.1f, 0.5f,  0.4f, 0.4f, 0.4f,  100.0f }
    };

    const Uniform<Material> material(color, 2);

    //�^�C�}�[��0�ɃZ�b�g
    glfwSetTime(0.0);

    // �E�B���h�E���J���Ă���ԌJ��Ԃ�
    while (window)
    {
        // �E�B���h�E����������
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // �V�F�[�_�v���O�����̎g�p�J�n
        glUseProgram(program);

        //�������ϊ��s������߂�
        const GLfloat* const size(window.getSize());
        const GLfloat fovy(window.getScale() * 0.01f);
        const GLfloat aspect(size[0] / size[1]);
        const Matrix projection(Matrix::perspective(fovy, aspect, 1.0f, 10.0f));


        //���s�ړ��̕ϊ��s������߂�
        const GLfloat* const position(window.getLocation());
        const Matrix translation(Matrix::translate(position[0], position[1], 0.0f));

        //���f���̕ϊ��s������߂�
        const GLfloat* const location(window.getLocation());
        const Matrix r(Matrix::rotate(static_cast<GLfloat>(glfwGetTime()), 0.0f, 1.0f, 0.0f));
        const Matrix model(Matrix::translate(location[0], location[1], 0.0f) * r);

        //�r���[�ϊ��s������߂�
        const Matrix view(Matrix::lookat(3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

        //�@���x�N�g���̕ϊ��s��̊i�[��
        GLfloat normalMatrix[9];

        //���f���r���[�ϊ��s������߂�
        const Matrix modelview(view * model);

        //���f���r���[���� �@���x�N�g���̕ϊ��s������߂� 
        modelview.getNormalMatrix(normalMatrix);

        // uniform �ϐ��ɒl��ݒ肷��
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.data());
        glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview.data());
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, normalMatrix);
        for(int i = 0; i < Lcount; ++i)
            glUniform4fv(LposLoc + i, 1, (view * Lpos[i]).data());
        glUniform3fv(LambLoc, Lcount, Lamb);
        glUniform3fv(LdiffLoc, Lcount, Ldiff);
        glUniform3fv(LspecLoc, Lcount, Lamb);

        material.select(0, 0);
        // �}�`��`�悷��
        shape->draw();


        //��ڂ̐}�`�`��
        const Matrix modelview1(modelview * Matrix::translate(0.0f, 0.0f, 3.0f));
        modelview1.getNormalMatrix(normalMatrix);
        glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview1.data());
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, normalMatrix);
        material.select(0, 1);
        shape->draw();

        // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
        window.swapBuffers();

    }
}