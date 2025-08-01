#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <GLFW/glfw3.h>

#include "stb_image.h"
using namespace std;


GLuint Load_PNG_Texture(const char* filename)
{
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);

    if (!data) {
        cout << "テクスチャを読み込めません: " << filename << endl;
        return 0;
    }

    cout << "テクスチャ読み込み成功: " << filename
        << " (" << width << "x" << height << ", " << channels << "ch)" << endl;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // テクスチャパラメータの設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // チャンネル数に応じてフォーマットを選択
    GLenum format;
    if (channels == 1)
        format = GL_LUMINANCE;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;
    else {
        cout << "未対応のチャンネル数: " << channels << endl;
        stbi_image_free(data);
        return 0;
    }

    // テクスチャデータの転送
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return textureID;
}
