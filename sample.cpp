#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>


//�V�F�[�_�̃\�[�X�t�@�C����ǂݍ��񂾃�������Ԃ�
//name:�V�F�[�_�̃\�[�X�t�F�C����
//buffer:�ǂݍ��񂾃\�[�X�t�@�C���̃e�L�X�g
bool readShaderSource(const char* name, vector<GLchar>& buffer)
{
	if (name == NULL) return false;

	//�\�[�X�t�@�C�����J��
	ifstream file(name, ios::binary);
	if (file.fail())
	{
		cout << "Error: Cannot open sourcefile:" << name << endl;
		return false;
	}

	//�t�@�C���̖����Ɉړ������݂̈ʒu�i�t�@�C���T�C�Y�j�𓾂�
	file.seekg(0L, ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());

	//�t�@�C���T�C�Y�̃������m��
	buffer.resize(length + 1);

	//�t�@�C����擪����ǂݍ���
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


//�V�F�[�_�[�I�u�W�F�N�g�̃R���p�C�����ʂ�\������
//shader:�V�F�[�_�[�I�u�W�F�N�g��
//str:�R���p�C���G���[�����������ꏊ������������
GLboolean printShaderInfoLog(GLuint shader, const char* str)
{
	//�R���p�C�����ʂ��擾
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) cout << "Compile Error in" << endl;

	//�V�F�[�_�[�̃R���p�C�����̃��O�̒������擾
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		//�V�F�[�_�[�̃R���p�C�����̃��O�̓��e���擾����
		vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		cout << &infoLog[0] << endl;
	}

	return static_cast<GLboolean>(status);
}


//�v���O�����I�u�W�F�N�g�̃����N���ʂ�\������
//program:�v���O�����I�u�W�F�N�g��
GLboolean printProgramInfoLog(GLuint program)
{
	//�����N���ʂ��擾����
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == NULL) cout << "Link Error" << endl;

	//�V�F�[�_�̃����N���̃��O�̒������擾
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		//�V�F�[�_�̃����N���̃��O�̓��e���擾����
		vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
		cout << &infoLog[0] << endl;
	}

	return static_cast<GLboolean>(status);
}


//�v���O�����I�u�W�F�N�g���쐬����
//vsrc:�o�[�e�b�N�X�V�F�[�_�[�̃\�[�X�v���O�����̕�����
//fsrc:�t���O�����g�V�F�[�_�[�̃\�[�X�v���O�����̕�����
GLuint createProgram(const char* vsrc, const char* fsrc)
{	
	//��̃v���O�����I�u�W�F�N�g���쐬����
	const GLuint program(glCreateProgram());

	if (vsrc != NULL)
	{
		//�o�[�e�b�N�X�V�F�[�_�[
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);

		/*
		�o�[�e�b�N�X�V�F�[�_�[�̃V�F�[�_�[�I�u�W�F�N�g��
		�v���O�����I�u�W�F�N�g�ɑg�ݍ���
		*/
		if (printShaderInfoLog(vobj, "vertex shader")) glAttachShader(program, vobj);
		glDeleteShader(vobj);
	}

	if (fsrc != NULL)
	{
		//�t���O�����g�V�F�[�_�[
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);

		if (printShaderInfoLog(fobj, "fragment shader")) glAttachShader(program, fobj);
		glDeleteShader(fobj);
	}

	//�v���O�����I�u�W�F�N�g�������N����
	glBindAttribLocation(program, 0, "position");
	glBindFragDataLocation(program, 0, "fragment");
	glLinkProgram(program);

	if(printProgramInfoLog(program)) return program;

	//�v���O�����I�u�W�F�N�g���쐬�ł��Ȃ����0��Ԃ�
	glDeleteProgram(program);
	return 0;
}


// �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���Ńv���O�����I�u�W�F�N�g���쐬����
// vert: �o�[�e�b�N�X�V�F�[�_�̃\�[�X�t�@�C����
// frag: �t���O�����g�V�F�[�_�̃\�[�X�t�@�C����
GLuint loadProgram(const char* vert, const char* frag)
{
	//�V�F�[�_�̃\�[�X�t�@�C����ǂݍ���
	vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));

	//�v���O�����I�u�W�F�N�g�쐬
	return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;

}


int main() {
	//GLFW������������
	if (glfwInit() == GL_FALSE) {
		cout << "Cannot initialize GLFW" << endl;
		return 1;
	}

	//�v���O�����I�����̏�����o�^����
	atexit(glfwTerminate);

	//OpenGLversion 3.2 core Proflie ��I������
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//�E�B���h�E���쐬����
	GLFWwindow *const window(glfwCreateWindow(640, 480, "Hello!", NULL, NULL));
	if (window == NULL) {
		cout << "Cannot create GLFW window" << endl;
		return 1;
	}

	//�쐬�����E�B���h�E��OpenGL�̏����Ώۂɂ���
	glfwMakeContextCurrent(window);

	//GLEW������������
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Cannot initialize GLEW" << endl;
		return 1;
	}

	// ���������̃^�C�~���O��҂�
	glfwSwapInterval(1);

	//�w�i�F���w�肷��
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	////�o�[�e�b�N�X�V�F�[�_�[�̃\�[�X�v���O����
	//static constexpr GLchar vsrc[] =
	//	"#version 150 core\n"
	//	"in vec4 position;\n"
	//	"void main()\n"
	//	"{"
	//	"	gl_Position = position;\n"
	//	"}\n";

	////�t���O�����g�V�F�[�_�[�̃\�[�X�v���O����
	//static constexpr GLchar fsrc[] =
	//	"#version 150 core\n"
	//	"out vec4 fragment;\n"
	//	"void main()\n"
	//	"{\n"
	//	"	fragment = vec4(1.0, 0.0, 0.0, 1.0};\n"
	//	"}\n";

	//�v���O�����I�u�W�F�N�g���쐬����
	const GLuint program(loadProgram("point.vert", "point.frag"));

	while (glfwWindowShouldClose(window) == GL_FALSE)
	{
		//�E�B���h�E��h��Ԃ�
		glClear(GL_COLOR_BUFFER_BIT);

		//�V�F�[�_�[�v���O�����̎g�p�J�n

		//�J���[�o�b�t�@�����ւ���
		glfwSwapBuffers(window);

		//�C�x���g�����o��
		glfwWaitEvents();
	}
}
