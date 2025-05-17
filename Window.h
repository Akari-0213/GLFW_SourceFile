#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std;

class Window {
	//ウィンドウのハンドル
	GLFWwindow* const window;

	//ウィンドウのサイズ
	GLfloat size[2];
	
	//ワールド座標系に対するデバイス座標系の拡大率
	GLfloat scale;

public:
	Window(int width = 640, int height = 480, const char* title = "Hello!")
		:window(glfwCreateWindow(width, height, title, NULL, NULL))
		,scale(10.0f)
	{
		if (window == NULL)
		{
			cout << "Cannot create GLFW window" << endl;
			exit(1);
		}

		//現在のウィンドウを処理対象にする
		glfwMakeContextCurrent(window);

		//GLEWを初期化する
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			cout << "Cannot initialize GLEW" << endl;
			exit(1);
		}

		// 垂直同期のタイミングを待つ
		glfwSwapInterval(1);

		//このインスタンスのthisポインタを記録しておく
		glfwSetWindowUserPointer(window, this);

		//ウィンドウのサイズ変更時に呼び出す処理の登録
		glfwSetWindowSizeCallback(window, resize);

		//開いたウィンドウの初期設定
		resize(window, width, height);
	}

	virtual ~Window() {
		glfwDestroyWindow(window);
	}

	//描画ループの継続判定
	explicit operator bool()
	{
		//イベントを取り出す
		glfwWaitEvents();

		//ウィンドウを閉じる必要がなければtrueを返す
		return !glfwWindowShouldClose(window);
	}

	//ダブルバッファリング
	void swapBuffers() const
	{
		//カラーバッファを入れ替える
		glfwSwapBuffers(window);
	}
	
	static void resize(GLFWwindow* const window, int width, int height)
	{
		//フレームバッファサイズを調べる
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

		//フレームバッファ全体をビューポートに設定
		glViewport(0, 0, fbWidth, fbHeight);

		//このインスタンスのthisポインタを得る
		Window* const
			instance(static_cast<Window*>(glfwGetWindowUserPointer(window)));

		if (instance != NULL) {
			//このインスタンスが保持する縦横比を更新する
			instance->size[0] = static_cast<GLfloat>(width); 
			instance->size[1] = static_cast<GLfloat>(height);
		}
	}

	//ウィンドウのサイズを取り出す
	const GLfloat *getSize() const { return size; }

	// ワールド座標系に対するデバイス座標系の拡大率を取り出す
	GLfloat getScale() const { return scale; }

};
