#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

#include <GL/glew.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

// using namespace glm;
using namespace std;

GLuint program;			//shader program

// Read shader file
std::string LoadShaderSource(const char file[])
{
	std::ifstream ifs(file);
	std::string content;
	if(ifs)
	{
		content.assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())); // read contents
	}
	return content;
}

void ShaderLog(GLuint shader)
{
	GLint isCompiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete[] errorLog;
	}
	else
	{
		printf("Succeeded to compile the shader\n");
	}
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glewInit();
	//Initialize shaders
	///////////////////////////
	program = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	static std::string vss = LoadShaderSource("vertex.vs.glsl");
	const char *vsSource = vss.c_str();
	static std::string fss = LoadShaderSource("fragment.fs.glsl");
	const char *fsSource = fss.c_str();
	glShaderSource(vs, 1, &vsSource, NULL);
	glShaderSource(fs, 1, &fsSource, NULL);
	glCompileShader(vs);
	glCompileShader(fs);
	ShaderLog(vs);
	ShaderLog(fs);

	//Attach Shader to program
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	glUseProgram(program);
	///////////////////////////

	float data[18] =
	{
		-0.5f, -0.4f, 0.0f,		//Position
		0.5f, -0.4f, 0.0f,
		0.0f, 0.6f, 0.0f,

		1.0f, 0.0f, 0.0f,		//Color
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	//The type used for names in OpenGL is GLuint
	GLuint buffer;
	//Generate a name dor the buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);


	//產生vao
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//分配屬性資料，座標與顏色的index分別為0,1
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(float) * 9));//offset

	// //啟用頂點屬性陣列，預設為false，因此要打開
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void drawTriangle()
{
	//Update shaders' input variable
	glDrawArrays(GL_TRIANGLES, 0, 3);
}


int main(int argc, char *argv[])
{
	bool running = true;
	sf::Clock deltaClock;
	sf::Event event;
	sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL");
	window.setVerticalSyncEnabled(true);
	window.setActive(true);
	// Init
	init();
	glClearColor(0.0, 0.0, 0.0, 1.0);

	while (running)
	{
		while (window.pollEvent(event))
		{
			// ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
			{
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				// adjust the viewport when the window is resized
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}
		// ImGui::SFML::Update(window, deltaClock.restart());
		// ImGui Update
		// ImGui::Begin("Debug");
		// ImGui::End();
		/////////////////////////////////////////////////////////////////
		// Draw
		//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawTriangle();
		// window.pushGLStates();
			// ImGui::SFML::Render(window);
		// window.popGLStates();
		window.display();
	}
	return 0;
}