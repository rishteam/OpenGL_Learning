#include "../../Include/Common.h"
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

using namespace std;

GLuint			square_buffer;
GLuint			square_vao;
GLuint			program;

static const GLfloat square_vertices[] =
{
	-0.4f, -0.4f, 0.0f, 1.0f,
	 0.4f, -0.4f, 0.0f, 1.0f,
	 0.4f, 0.4f, 0.0f, 1.0f,
	-0.4f, 0.4f, 0.0f, 1.0f
};

const char* vsSource = R"glsl(
	#version 400

	layout (location = 0) in vec4 position;

	void main()
	{
		gl_Position = position;
	}
)glsl";
const char* fsSource = R"glsl(
	#version 400

	out vec4 color;

	void main()
	{
		color = vec4(1.0, 0.0, 0.0, 1.0);
	}
)glsl";

void My_Init()
{
	//清除顏色
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//初始化glew
	glewInit();
	//Initialize shaders
	//產生頂點渲染器
	//產生片段渲染器
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	//要換成code的渲染器物件
	//程式碼由幾段字串組成
	//要綁定的字元陣列
	//對應的程式碼長度
	glShaderSource(vs, 1, &vsSource, NULL);
	glShaderSource(fs, 1, &fsSource, NULL);

	//編譯shader
	glCompileShader(vs);
	glCompileShader(fs);

	//Attach Shader to program
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	///////////////////////////
	//設定頂點資料
	glGenVertexArrays(1, &square_vao);
	glBindVertexArray(square_vao);
	glGenBuffers(1, &square_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, square_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices) * sizeof(float), &square_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	//清除暫存器
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update shaders' input variable
	///////////////////////////	
	//使用program
	glUseProgram(program);
	{
		//將頂點資料傳入render pipeline
		glBindVertexArray(square_vao);
		//每三個頂點之間畫三角形，從buffer的哪個位置開始畫，頂點數量
		glDrawArrays(GL_QUADS, 0, 4);
	}
	///////////////////////////	

}

int main(int argc, char *argv[])
{
	sf::Window window(sf::VideoMode(800, 600), "OpenGL");
	window.setVerticalSyncEnabled(true);
	//check close
	bool running = true;
	// activate the window
	window.setActive(true);

	while (running)
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				// end the program
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				// adjust the viewport when the window is resized
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}
		//Call custom initialize function
		My_Init();
		//Register GLUT callback functions
		My_Display();
		window.display();
	}
	return 0;
}