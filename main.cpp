#include <iostream>
#include <vector>
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
#include "Shader.h"
#include "Load_ObjFile.h"
#include "Load_Texture.h"
#include "ModelData.h"
#include "ModelData_static.h"
#include "ParticleSystem.h"


#define TimeStep 0.1f


void drawParticle(ParticleSystem& particleSystem, GLfloat* player_position, const GLuint particle_program, float deltaTime,
    const GLint particle_modelviewLoc, const GLint particle_projectionLoc, const Matrix view, const Matrix projection,
    const GLint particle_TexLoc)
{
    // パーティクルエミッターの位置を車の位置に追従させる
    particleSystem.setEmitterPosition(glm::vec3(0.0f, -1.0f, 2.0f));

    // パーティクルシステムを更新
    particleSystem.Update(deltaTime);
    glUseProgram(particle_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, particleSystem.getTexture());
    glUniform1f(particle_TexLoc, 0);

    // パーティクルの描画
    glUniformMatrix4fv(particle_modelviewLoc, 1, GL_FALSE, view.data());
    glUniformMatrix4fv(particle_projectionLoc, 1, GL_FALSE, projection.data());
    particleSystem.Render();
}


int main()
{
    // GLFW を初期化する
    if (glfwInit() == GL_FALSE)
    {
        std::cerr << "Can't initialize GLFW" << std::endl;
        return 1;
    }
    glewExperimental = GL_TRUE;
    // GLEW を初期化する
    if (glewInit() == GL_FALSE)
    {
        std::cerr << "Can't initialize GLEW" << std::endl;
        return 1;
    }

    // プログラム終了時の処理を登録する
    atexit(glfwTerminate);

    // OpenGL Version 3.2 Core Profile を選択する
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ウィンドウを作成する
    Window window;

    //モデル読み込み
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

    GLfloat player_position[3] = { 0.0f, 0.0f, 0.0f };
    GLfloat player_rotation_angle = 0.0f;

    // 背景色を指定する
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    //背面カリングを有効にする
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // デプスバッファを有効にする 
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    // プログラムオブジェクトを作成する
    const GLuint program(loadProgram("point.vert", "point.frag"));
    const GLuint particle_program(loadParticleProgram("particle.vert", "particle.frag"));

    // uniform 変数の場所を取得する
    const GLint modelviewLoc(glGetUniformLocation(program, "modelview"));
    const GLint projectionLoc(glGetUniformLocation(program, "projection"));
    const GLint normalMatrixLoc(glGetUniformLocation(program, "normalMatrix"));
    const GLint LposLoc(glGetUniformLocation(program, "Lpos"));
    const GLint LambLoc(glGetUniformLocation(program, "Lamb"));
    const GLint LdiffLoc(glGetUniformLocation(program, "Ldiff"));
    const GLint LspecLoc(glGetUniformLocation(program, "Lspec"));
    const GLint TexLoc = glGetUniformLocation(program, "tex_sample");

    const GLint particle_modelviewLoc(glGetUniformLocation(particle_program, "modelview"));
    const GLint particle_projectionLoc(glGetUniformLocation(particle_program, "projection"));
    const GLint particle_TexLoc = glGetUniformLocation(particle_program, "particle_tex");

    // uniform block の場所を取得する
    const GLint materialLoc(glGetUniformBlockIndex(program, "Material"));

    // uniform block の場所を 0 番の結合ポイントに結びつける 
    glUniformBlockBinding(program, materialLoc, 0);

    // パーティクルシステムの設定
    ParticleSystem particleSystem(500);
    particleSystem.Initialize();
    particleSystem.setEmitterPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    particleSystem.setEmitterDirection(glm::vec3(0.0f, 1.0f, 0.0f));
    particleSystem.setEmitterRate(20.0f); 
    particleSystem.setParticleLifeTime(0.5f);
    particleSystem.setAirResistance(0.8f);
    particleSystem.setGravity(9.8f);
    GLuint particleTexture = Load_PNG_Texture("particle_lawn.png");
    particleSystem.setTexture(particleTexture);

    //光源データ
    static constexpr int Lcount(2);
    static constexpr Vector_light Lpos[] = { 0.0f, 0.0f, 100.0f, 1.0f, 
                                             0.0f, 100.0f, 0.0f, 1.0f };
    static constexpr GLfloat Lamb[] = { 0.5f, 0.5f, 0.5f, 
                                        0.1f, 0.1f, 0.1f };
    static constexpr GLfloat Ldiff[] = { 0.5f, 0.5f, 0.5f, 
                                         0.9f, 0.9f, 0.9f };
    static constexpr GLfloat Lspec[] = { 0.5f, 0.5f, 0.5f, 
                                         0.9f, 0.9f, 0.9f };
    static constexpr Material color[] =
    {
        { 0.5f, 0.5f, 0.5f,  0.9f, 0.9f, 0.9f,  0.3f, 0.3f, 0.3f,  30.0f },
        { 0.5f, 0.5f, 0.5f,  0.9f, 0.9f, 0.9f,  0.4f, 0.4f, 0.4f,  100.0f }
    };

    const Uniform<Material> material(color, 3);

    const float cameraDistance = 5.0f;
    const float cameraHeight = 3.0f;
    const float cameraAngle = 0.0f;

    glfwSetTime(0.0);
    double lastTime = glfwGetTime();
    bool is_moveing = false;

    // ウィンドウが開いている間繰り返す
    while (window)

    {   // デルタタイムを計算
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        is_moveing = false;

        // ウィンドウを消去する
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // シェーダプログラムの使用開始
        glUseProgram(program);

        GLFWwindow* glfw_window = window.getWindow();

        //キーボード入力
        if (glfwGetKey(glfw_window, GLFW_KEY_W) != GLFW_RELEASE)
        {
            player_position[2] -= cos(player_rotation_angle) * TimeStep;
            player_position[0] -= sin(player_rotation_angle) * TimeStep;
            is_moveing = true;
        }
        else if (glfwGetKey(glfw_window, GLFW_KEY_S) != GLFW_RELEASE)
        {
            player_position[2] += cos(player_rotation_angle) * TimeStep;
            player_position[0] += sin(player_rotation_angle) * TimeStep;
            is_moveing = true;
        }
        if (glfwGetKey(glfw_window, GLFW_KEY_A) != GLFW_RELEASE)
            player_rotation_angle += TimeStep * 0.1f;
        else if (glfwGetKey(glfw_window, GLFW_KEY_D) != GLFW_RELEASE)
            player_rotation_angle -= TimeStep * 0.1f;

        const float cameraX = player_position[0] + cameraDistance * sin(cameraAngle);
        const float cameraZ = player_position[2] + cameraDistance * cos(cameraAngle);
        const float cameraY = player_position[1] + cameraHeight;

        //透視投変換行列を求める
        const GLfloat* const size(window.getSize());
        const GLfloat fovy(window.getScale() * 0.01f);
        const GLfloat aspect(size[0] / size[1]);
        const Matrix projection(Matrix::perspective(fovy, aspect, 1.0f, 50.0f));

        //モデルの変換行列を求める
        const GLfloat* const location(window.getLocation());

        //ビュー変換行列を求める
        const Matrix view(Matrix::lookat(cameraX, cameraY, cameraZ, player_position[0], player_position[1], player_position[2], 0.0f, 1.0f, 0.0f));

        //モデルビュー変換行列を求める
        Matrix rotation = Matrix::rotate(static_cast<GLfloat>(player_rotation_angle), 0.0f, 1.0f, 0.0f);
        Matrix modelMatrix = Matrix::translate(player_position[0], player_position[1], player_position[2]) * rotation;
        const Matrix modelview(view * modelMatrix);

        //静止オブジェクトのモデルビュー変換行列を求める
        Matrix static_modelMatrix = Matrix::translate(0.0f, 0.0f, 0.0f);
        const Matrix static_modelview(view * static_modelMatrix);

        // uniform 変数に値を設定する
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.data());
        for(int i = 0; i < Lcount; ++i)
            glUniform4fv(LposLoc + i, 1, (view * Lpos[i]).data());
        glUniform3fv(LambLoc, Lcount, Lamb);
        glUniform3fv(LdiffLoc, Lcount, Ldiff);
        glUniform3fv(LspecLoc, Lcount, Lamb);
        glUniform1i(TexLoc, 0);
        
        GLfloat player_normalMatrix[9];
        modelview.getNormalMatrix(player_normalMatrix);

        GLfloat static_normalMatrix[9];
        static_modelview.getNormalMatrix(static_normalMatrix);

        for (size_t i = 0; i < models.size(); ++i)
        {
            const ModelData& model = *models[i];

            glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview.data());
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, player_normalMatrix);

            //テクスチャをバインドする
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model.model_st.texture);

            material.select(0, i);
            // 図形を描画する
            model.model_st.shape->draw();

        }
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        for (size_t i = 0; i < static_models.size(); ++i)
        {

            const ModelData_static& static_model = *static_models[i];

            glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, static_modelview.data());
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, static_normalMatrix);

            //テクスチャをバインドする
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, static_model.model_st.texture);

            material.select(0, i);
            // 図形を描画する
            static_model.model_st.shape->draw();

        }
        glDepthMask(GL_TRUE);
        if (is_moveing) {
            drawParticle(particleSystem, player_position, particle_program, deltaTime, particle_modelviewLoc, particle_projectionLoc, modelview, projection, particle_TexLoc);
        }
        // カラーバッファを入れ替えてイベントを取り出す
        window.swapBuffers();
    }
    
}
