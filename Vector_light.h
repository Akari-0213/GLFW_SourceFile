#pragma once
#include <array>

//変換行列
#include "Matrix.h"

//ベクトル
using Vector_light = std::array<GLfloat, 4>;


Vector_light operator*(const Matrix& m, const Vector_light& v)
{
	Vector_light t;

	for (int i = 0; i < 4; ++i)
	{
		t[i] = m[i] * v[0] + m[i + 4] * v[1] + m[i + 8] * v[2] + m[i + 12] * v[3];
	}

	return t;
}
