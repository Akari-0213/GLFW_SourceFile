#pragma once
#include <iostream>
#include <vector>
#include <string>
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


class Load_ObjFile {

public:
	vector<Vertex> vertices;
	vector<Normal> normals;
	vector<Texcoord> texcoords;

	vector<vector<FaceVertex>> obj_faces;

	Load_ObjFile(){}
	virtual ~Load_ObjFile(){}
	void FileScan(const string& obj_name);

private:
	//コピーインストラクタによるコピー禁止
	Load_ObjFile(const Load_ObjFile& m) = delete;
	//代入によるコピー禁止
	Load_ObjFile& operator=(const Load_ObjFile& m) = delete;
};
