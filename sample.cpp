#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>


//シェーダのソースファイルを読み込んだメモリを返す
//name:シェーダのソースフェイル名
//buffer:読み込んだソースファイルのテキスト
bool readShaderSource(const char* name, vector<GLchar>& buffer)
{
	if (name == NULL) return false;

	//ソースファイルを開く
	ifstream file(name, ios::binary);
	if (file.fail())
	{
		cout << "Error: Cannot open sourcefile:" << name << endl;
		return false;
	}

	//ファイルの末尾に移動し現在の位置（ファイルサイズ）を得る
	file.seekg(0L, ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());

	//ファイルサイズのメモリ確保
	buffer.resize(length + 1);

	//ファイルを先頭から読み込む
	file.seekg(0L, ios::beg);
	file.read(buffer.data(), length);
	buffer[length] = '\0';

	if (file.fail())
	{
		cout << "Error: Could not read Sourcefile:" << name << endl;
		file.close();
		return false;
	}

	file.close();
	return true;
}


//シェーダーオブジェクトのコンパイル結果を表示する
//shader:シェーダーオブジェクト名
//str:コンパイルエラーが発生した場所を示す文字列
GLboolean printShaderInfoLog(GLuint shader, const char* str)
{
	//コンパイル結果を取得
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) cout << "Compile Error in" << endl;

	//シェーダーのコンパイル時のログの長さを取得
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		//シェーダーのコンパイル時のログの内容を取得する
		vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		cout << &infoLog[0] << endl;
	}

	return static_cast<GLboolean>(status);
}


//プログラムオブジェクトのリンク結果を表示する
//program:プログラムオブジェクト名
GLboolean printProgramInfoLog(GLuint program)
{
	//リンク結果を取得する
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == NULL) cout << "Link Error" << endl;

	//シェーダのリンク時のログの長さを取得
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		//シェーダのリンク時のログの内容を取得する
		vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
		cout << &infoLog[0] << endl;
	}

	return static_cast<GLboolean>(status);
}


//プログラムオブジェクトを作成する
//vsrc:バーテックスシェーダーのソースプログラムの文字列
//fsrc:フラグメントシェーダーのソースプログラムの文字列
GLuint createProgram(const char* vsrc, const char* fsrc)
{	
	//空のプログラムオブジェクトを作成する
	const GLuint program(glCreateProgram());

	if (vsrc != NULL)
	{
		//バーテックスシェーダー
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);

		/*
		バーテックスシェーダーのシェーダーオブジェクトを
		プログラムオブジェクトに組み込む
		*/
		if (printShaderInfoLog(vobj, "vertex shader")) glAttachShader(program, vobj);
		glDeleteShader(vobj);
	}

	if (fsrc != NULL)
	{
		//フラグメントシェーダー
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);

		if (printShaderInfoLog(fobj, "fragment shader")) glAttachShader(program, fobj);
		glDeleteShader(fobj);
	}

	//プログラムオブジェクトをリンクする
	glBindAttribLocation(program, 0, "position");
	glBindFragDataLocation(program, 0, "fragment");
	glLinkProgram(program);

	if(printProgramInfoLog(program)) return program;

	//プログラムオブジェクトが作成できなければ0を返す
	glDeleteProgram(program);
	return 0;
}


// シェーダのソースファイルを読み込んでプログラムオブジェクトを作成する
// vert: バーテックスシェーダのソースファイル名
// frag: フラグメントシェーダのソースファイル名
GLuint loadProgram(const char* vert, const char* frag)
{
	//シェーダのソースファイルを読み込む
	vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));

	//プログラムオブジェクト作成
	return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;

}


int main() {
	//GLFWを初期化する
	if (glfwInit() == GL_FALSE) {
		cout << "Cannot initialize GLFW" << endl;
		return 1;
	}

	//プログラム終了時の処理を登録する
	atexit(glfwTerminate);

	//OpenGLversion 3.2 core Proflie を選択する
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//ウィンドウを作成する
	GLFWwindow *const window(glfwCreateWindow(640, 480, "Hello!", NULL, NULL));
	if (window == NULL) {
		cout << "Cannot create GLFW window" << endl;
		return 1;
	}

	//作成したウィンドウをOpenGLの処理対象にする
	glfwMakeContextCurrent(window);

	//GLEWを初期化する
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Cannot initialize GLEW" << endl;
		return 1;
	}

	// 垂直同期のタイミングを待つ
	glfwSwapInterval(1);

	//背景色を指定する
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	////バーテックスシェーダーのソースプログラム
	//static constexpr GLchar vsrc[] =
	//	"#version 150 core\n"
	//	"in vec4 position;\n"
	//	"void main()\n"
	//	"{"
	//	"	gl_Position = position;\n"
	//	"}\n";

	////フラグメントシェーダーのソースプログラム
	//static constexpr GLchar fsrc[] =
	//	"#version 150 core\n"
	//	"out vec4 fragment;\n"
	//	"void main()\n"
	//	"{\n"
	//	"	fragment = vec4(1.0, 0.0, 0.0, 1.0};\n"
	//	"}\n";

	//プログラムオブジェクトを作成する
	const GLuint program(loadProgram("point.vert", "point.frag"));

	while (glfwWindowShouldClose(window) == GL_FALSE)
	{
		//ウィンドウを塗りつぶす
		glClear(GL_COLOR_BUFFER_BIT);

		//シェーダープログラムの使用開始

		//カラーバッファを入れ替える
		glfwSwapBuffers(window);

		//イベントを取り出す
		glfwWaitEvents();
	}
}
