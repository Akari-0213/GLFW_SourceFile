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
#include "Load_ObjFile.h"
#include "Load_Texture.h"
#include "ModelData.h"
#include "ModelData_static.h"
#include "ParticleSystem.h"

#define PI 3.14159
#define TimeStep 0.1f

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
    glBindAttribLocation(program, 2, "texcoord");
    glBindFragDataLocation(program, 0, "fragment");
    glLinkProgram(program);

    // �쐬�����v���O�����I�u�W�F�N�g��Ԃ�
    if (printProgramInfoLog(program))
        return program;

    // �v���O�����I�u�W�F�N�g���쐬�ł��Ȃ���� 0 ��Ԃ�
    glDeleteProgram(program);
    return 0;
}


// �v���O�����I�u�W�F�N�g���쐬����
//   vsrc: �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O�����̕�����
//   fsrc: �t���O�����g�V�F�[�_�̃\�[�X�v���O�����̕�����
GLuint createParticleProgram(const char* vsrc, const char* fsrc)
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
    glBindAttribLocation(program, 0, "vertex_position");
    glBindAttribLocation(program, 1, "instance_position");
    glBindAttribLocation(program, 2, "instance_color");
    glBindAttribLocation(program, 3, "instance_size");
    glBindAttribLocation(program, 4, "instance_rotation");
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


GLuint loadParticleProgram(const char* vert, const char* frag)
{
    // �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���
    std::vector<GLchar> vsrc_particle;
    const bool vstat(readShaderSource(vert, vsrc_particle));
    std::vector<GLchar> fsrc_particle;
    const bool fstat(readShaderSource(frag, fsrc_particle));

    // �v���O�����I�u�W�F�N�g���쐬����
    return vstat && fstat ? createParticleProgram(vsrc_particle.data(), fsrc_particle.data()) : 0;
}


void drawParticle(ParticleSystem& particleSystem, GLfloat* groupPosition, const GLuint particle_program, float deltaTime,
    const GLint particle_modelviewLoc, const GLint particle_projectionLoc, const Matrix view, const Matrix projection,
    const GLint particle_TexLoc)
{
    

    // �p�[�e�B�N���G�~�b�^�[�̈ʒu���Ԃ̈ʒu�ɒǏ]������
    particleSystem.setEmitterPosition(glm::vec3(0.0f, -1.0f, 2.0f));

    // �p�[�e�B�N���V�X�e�����X�V
    particleSystem.Update(deltaTime);
    glUseProgram(particle_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, particleSystem.getTexture());
    glUniform1f(particle_TexLoc, 0);

    // �p�[�e�B�N���̕`��
    glUniformMatrix4fv(particle_modelviewLoc, 1, GL_FALSE, view.data());
    glUniformMatrix4fv(particle_projectionLoc, 1, GL_FALSE, projection.data());
    particleSystem.Render();
}

int main()
{
    // GLFW ������������
    if (glfwInit() == GL_FALSE)
    {
        std::cerr << "Can't initialize GLFW" << std::endl;
        return 1;
    }
    glewExperimental = GL_TRUE;
    // GLEW ������������
    if (glewInit() == GL_FALSE)
    {
        std::cerr << "Can't initialize GLEW" << std::endl;
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

    //���f���ǂݍ���
    vector<unique_ptr<ModelData>> models;
    vector<unique_ptr<ModelData_static>> static_models;

    auto model1 = make_unique<ModelData>();
    model1->ModelLoad("chiikawa.obj", "chiikawa_face.png");
    model1->model_st.transform = Matrix::translate(0.0f, 0.0f, 0.0f);
    models.push_back(move(model1));


    auto model2 = make_unique<ModelData>();
    model2->ModelLoad("chiikawa_car.obj", "chiikawa_car_color.png");
    model2->model_st.transform = Matrix::translate(0.0f, 0.0f, 0.0f);
    models.push_back(move(model2));

    auto model_static1 = make_unique<ModelData_static>();
    model_static1->ModelLoad("chiikawa_lawn.obj", "chiikawa_lawn.png");
    model_static1->model_st.transform = Matrix::translate(0.0f, 0.0f, 0.0f);
    static_models.push_back(move(model_static1));

    GLfloat groupPosition[3] = { 0.0f, 0.0f, 0.0f };
    GLfloat groupRotationAngle = 0.0f;



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
    const GLuint particle_program(loadParticleProgram("particle.vert", "particle.frag"));

    // uniform �ϐ��̏ꏊ���擾����
    const GLint modelviewLoc(glGetUniformLocation(program, "modelview"));
    const GLint projectionLoc(glGetUniformLocation(program, "projection"));
    const GLint normalMatrixLoc(glGetUniformLocation(program, "normalMatrix"));
    const GLint LposLoc(glGetUniformLocation(program, "Lpos"));
    const GLint LambLoc(glGetUniformLocation(program, "Lamb"));
    const GLint LdiffLoc(glGetUniformLocation(program, "Ldiff"));
    const GLint LspecLoc(glGetUniformLocation(program, "Lspec"));
    const GLint TexLoc = glGetUniformLocation(program, "tex");


    const GLint particle_modelviewLoc(glGetUniformLocation(particle_program, "modelview"));
    const GLint particle_projectionLoc(glGetUniformLocation(particle_program, "projection"));
    const GLint particle_TexLoc = glGetUniformLocation(particle_program, "particle_texture");
   

    // uniform block �̏ꏊ���擾����
    const GLint materialLoc(glGetUniformBlockIndex(program, "Material"));

    // uniform block �̏ꏊ�� 0 �Ԃ̌����|�C���g�Ɍ��т��� 
    glUniformBlockBinding(program, materialLoc, 0);


    ParticleSystem particleSystem(500);
    particleSystem.Initialize();
    // �p�[�e�B�N���G�~�b�^�[�̐ݒ�
    particleSystem.setEmitterPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    particleSystem.setEmitterDirection(glm::vec3(0.0f, 1.0f, 0.0f));
    particleSystem.setEmitterRate(20.0f); // ���b5�̃p�[�e�B�N��
    particleSystem.setParticleLifeTime(1.0f); // 2�b�Ԑ���
    particleSystem.setAirResistance(0.8f);
    particleSystem.setGravity(9.8f);
    GLuint particleTexture = Load_PNG_Texture("particle_lawn.png");
    particleSystem.setTexture(particleTexture);

    // �f���^�^�C���v�Z�p
    double lastTime = glfwGetTime();

    //�����f�[�^
    static constexpr int Lcount(2);
    static constexpr Vector_light Lpos[] = { 0.0f, 0.0f, 10.0f, 1.0f, 10.0f, 0.0f, 0.0f, 1.0f };
    static constexpr GLfloat Lamb[] = { 0.5f, 0.5f, 0.5f, 0.1f, 0.1f, 0.1f };
    static constexpr GLfloat Ldiff[] = { 1.0f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f };
    static constexpr GLfloat Lspec[] = { 1.0f, 0.5f, 0.5f,  0.9f, 0.9f, 0.9f };

    static constexpr Material color[] =
    {
        { 0.5f, 0.5f, 0.5f,  0.9f, 0.9f, 0.9f,  0.3f, 0.3f, 0.3f,  30.0f },
        { 0.5f, 0.5f, 0.5f,  0.9f, 0.9f, 0.9f,  0.4f, 0.4f, 0.4f,  100.0f }
    };

    const Uniform<Material> material(color, 3);

    const float cameraDistance = 5.0f;
    const float cameraHeight = 3.0f;
    const float cameraAngle = 0.0f; //+3.14f�ŃJ�������ʉ�]

    glfwSetTime(0.0);
    lastTime = glfwGetTime();

    bool is_moveing = false;

    // �E�B���h�E���J���Ă���ԌJ��Ԃ�
    while (window)

    {   // �f���^�^�C�����v�Z
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        is_moveing = false;

        // �E�B���h�E����������
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // �V�F�[�_�v���O�����̎g�p�J�n
        glUseProgram(program);
       

        GLFWwindow* glfw_window = window.getWindow();
        if (glfwGetKey(glfw_window, GLFW_KEY_W) != GLFW_RELEASE)
        {
            groupPosition[2] -= cos(groupRotationAngle) * TimeStep;
            groupPosition[0] -= sin(groupRotationAngle) * TimeStep;
            is_moveing = true;

        }
        else if (glfwGetKey(glfw_window, GLFW_KEY_S) != GLFW_RELEASE)
        {
            groupPosition[2] += cos(groupRotationAngle) * TimeStep;
            groupPosition[0] += sin(groupRotationAngle) * TimeStep;
            is_moveing = true;
        }
        if (glfwGetKey(glfw_window, GLFW_KEY_A) != GLFW_RELEASE)
            groupRotationAngle += TimeStep * 0.1f;
        else if (glfwGetKey(glfw_window, GLFW_KEY_D) != GLFW_RELEASE)
            groupRotationAngle -= TimeStep * 0.1f;


        const float cameraX = groupPosition[0] + cameraDistance * sin(cameraAngle);
        const float cameraZ = groupPosition[2] + cameraDistance * cos(cameraAngle);
        const float cameraY = groupPosition[1] + cameraHeight;
        //�������ϊ��s������߂�
        const GLfloat* const size(window.getSize());
        const GLfloat fovy(window.getScale() * 0.01f);
        const GLfloat aspect(size[0] / size[1]);
        const Matrix projection(Matrix::perspective(fovy, aspect, 1.0f, 50.0f));

        //���s�ړ��̕ϊ��s������߂�
        const GLfloat* const position(window.getLocation());
        const Matrix translation(Matrix::translate(position[0], position[1], 0.0f));

        //���f���̕ϊ��s������߂�
        const GLfloat* const location(window.getLocation());

        //�r���[�ϊ��s������߂�
        const Matrix view(Matrix::lookat(cameraX, cameraY, cameraZ, groupPosition[0], groupPosition[1], groupPosition[2], 0.0f, 1.0f, 0.0f));

        Matrix rotation = Matrix::rotate(static_cast<GLfloat>(groupRotationAngle), 0.0f, 1.0f, 0.0f);
        Matrix modelMatrix = Matrix::translate(groupPosition[0], groupPosition[1], groupPosition[2]) * rotation;
        //���f���r���[�ϊ��s������߂�
        const Matrix modelview(view * modelMatrix);
        // uniform �ϐ��ɒl��ݒ肷��
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.data());
        for(int i = 0; i < Lcount; ++i)
            glUniform4fv(LposLoc + i, 1, (view * Lpos[i]).data());
        glUniform3fv(LambLoc, Lcount, Lamb);
        glUniform3fv(LdiffLoc, Lcount, Ldiff);
        glUniform3fv(LspecLoc, Lcount, Lamb);
        glUniform1i(TexLoc, 0);

        for (size_t i = 0; i < models.size(); ++i)
        {
            const ModelData& model = *models[i];
            


            //�@���x�N�g���̕ϊ��s��̊i�[��
            GLfloat normalMatrix[9];
            modelview.getNormalMatrix(normalMatrix);


            glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview.data());
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, normalMatrix);


            //�e�N�X�`�����o�C���h����
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model.model_st.texture);


            material.select(0, i);
            // �}�`��`�悷��
            model.model_st.shape->draw();

        }

        for (size_t i = 0; i < static_models.size(); ++i)
        {
            const ModelData_static& static_model = *static_models[i];

            //Matrix rotation = Matrix::rotate(static_cast<GLfloat>(glfwGetTime()), 0.0f, 1.0f, 0.0f);
            Matrix static_modelMatrix = Matrix::translate(0.0f, 0.0f, 0.0f);

            //���f���r���[�ϊ��s������߂�
            const Matrix static_modelview(view * static_modelMatrix);

            //�@���x�N�g���̕ϊ��s��̊i�[��
            GLfloat normalMatrix[9];
            static_modelview.getNormalMatrix(normalMatrix);


            glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, static_modelview.data());
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, normalMatrix);


            //�e�N�X�`�����o�C���h����
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, static_model.model_st.texture);


            material.select(0, i);
            // �}�`��`�悷��
            static_model.model_st.shape->draw();

        }
        if (is_moveing) {
            drawParticle(particleSystem, groupPosition, particle_program, deltaTime, particle_modelviewLoc, particle_projectionLoc, modelview, projection, particle_TexLoc);
        }

        // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
        window.swapBuffers();

    }
    
}