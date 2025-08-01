#pragma once
#include <array>
#include <GL/glew.h>

struct Material
{
	// �����̔��ˌW�� 
	alignas(16) std::array<GLfloat, 3> ambient;

	// �g�U���ˌW�� 
	alignas(16) std::array<GLfloat, 3> diffuse;

	// ���ʔ��ˌW�� 
	alignas(16) std::array<GLfloat, 3> specular;

	// �P���W�� 
	alignas(4) GLfloat shininess;
};