#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
using namespace std;

struct Vertex {
	glm::vec3 vertex_position;

	Vertex(const glm::vec3& pos)
		: vertex_position(pos) {
	}
};


struct Normal {
	glm::vec3 normal_vector;

	Normal(const glm::vec3& normal)
		: normal_vector(normal) {

	}
};


struct Texcoord {
	glm::vec2 texcoord;

	Texcoord(const glm::vec2& uv)
		: texcoord(uv) {
	}
};


struct FaceVertex {
	int vertex_index;
	int texCoord_index;
	int normal_index;
};
