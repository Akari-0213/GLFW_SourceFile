#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

#include "Load_ObjFile.h"
using namespace std;


void Load_ObjFile::FileScan(const string &obj_name) {
	ifstream file(obj_name);
	if (!file.is_open()) {
		cout << obj_name << "を開けません" << endl;
		return;
	}

	string line;
	while (getline(file, line)) {
		if (line.find("v ") == 0) {
			//頂点座標
			istringstream iss(line);
			string prefix;
			float x, y, z;
			if (iss >> prefix >> x >> y >> z) {
				vertices.push_back(Vertex(glm::vec3(x, y, z)));
			}
		}
		else if (line.find("vn ") == 0) {
			//法線ベクトル
			istringstream iss(line);
			string prefix;
			float x, y, z;
			if (iss >> prefix >> x >> y >> z) {
				normals.push_back(Normal(glm::vec3(x, y, z)));
			}
		}
		else if (line.find("vt ") == 0) {
			//テクスチャ座標
			istringstream iss(line);
			string prefix;
			float u, v;
			if (iss >> prefix >> u >> v) {
				texcoords.push_back(Texcoord(glm::vec2(u, 1.0f - v)));
			}
		}
		else if (line.find("f ") == 0) {
			istringstream iss(line);
			string prefix;
			iss >> prefix;

			vector<FaceVertex> face;
			string vertex_data;
			while (iss >> vertex_data) {
				FaceVertex fv = { -1, -1, -1 };

				// v/vt/vn形式のパース
				size_t firstSlash = vertex_data.find('/');
				if (firstSlash != string::npos) {
					fv.vertex_index = stoi(vertex_data.substr(0, firstSlash)) - 1; //0ベースに変換する

					size_t secondSlash = vertex_data.find('/', firstSlash + 1);
					if (secondSlash != string::npos) {
						if (secondSlash > firstSlash + 1) {
							fv.texCoord_index = stoi(vertex_data.substr(firstSlash + 1, secondSlash - firstSlash - 1)) - 1;
						}
						fv.normal_index = stoi(vertex_data.substr(secondSlash + 1)) - 1;
					}
					else {
						fv.texCoord_index = stoi(vertex_data.substr(firstSlash + 1)) - 1;
					}
				}
				else {
					fv.vertex_index = stoi(vertex_data) - 1;
				}
					
				face.push_back(fv);
			}
			obj_faces.push_back(face);
		
		}
	}
	file.close();
	cout << "読み込み完了: " << endl;
	cout << "頂点数: " << vertices.size() << endl;
	cout << "法線数: " << normals.size() << endl;
	cout << "テクスチャ座標数: " << texcoords.size() << endl;
	cout << "面数: " << obj_faces.size() << endl;
}

