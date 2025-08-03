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
        cout << "�e�N�X�`����ǂݍ��߂܂���: " << filename << endl;
        return 0;
    }

    cout << "�e�N�X�`���ǂݍ��ݐ���: " << filename
        << " (" << width << "x" << height << ", " << channels << "ch)" << endl;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // �e�N�X�`���p�����[�^�̐ݒ�
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // �`�����l�����ɉ����ăt�H�[�}�b�g��I��
    GLenum format;
    if (channels == 1)
        format = GL_LUMINANCE;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;
    else {
        cout << "���Ή��̃`�����l����: " << channels << endl;
        stbi_image_free(data);
        return 0;
    }

    // �e�N�X�`���f�[�^�̓]��
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return textureID;
}
