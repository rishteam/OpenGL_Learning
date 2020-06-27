#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

#include "shader_compile.h"

#include <GL/glew.h>
//OpenGL Mathematics
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

using namespace glm;
using namespace std;

GLuint			program;			//shader program
mat4 proj_matrix;					//projection matrix

int shader_now = 0;

GLuint hawk_texture;
GLint Shader_now_Loc;
int defalut_w = 1006;
int defalut_h = 600;

void init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glewInit();

	//Initialize shaders
	///////////////////////////
	program = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	static std::string vss = LoadShaderSource("./Image_Processing.vs.glsl");
	const char *vsSource = vss.c_str();
	static std::string fss = LoadShaderSource("./Image_Processing.fs.glsl");
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

	Shader_now_Loc = glGetUniformLocation(program, "shader_now");

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	float data[] = {
		1.0f,-1.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,0.0f,
		-1.0f,1.0f,0.0f,1.0f,
		1.0f,1.0f,1.0f,1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	TextureData tdata = Load_png("./hawk.png");

	glGenTextures(1, &hawk_texture);
	glBindTexture(GL_TEXTURE_2D, hawk_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glUniform1i(Shader_now_Loc, shader_now);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	///////////////////////////
}

//Reshape
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	proj_matrix = perspective(deg2rad(50.0f), viewportAspect, 0.1f, 100.0f);
}

// Normal 0
// Shader_Average_filter 1
// Shader_Median_filter 2
// Shader_Triangle_filter 3
// Shader_Gaussian_filter 4
// Shader_Quantization 5
// Shader_DoG 6
// Shader_Image_Abstraction 7
void key_input()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
	{
		shader_now = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
	{
		shader_now = 1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
	{
		shader_now = 2;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
	{
		shader_now = 3;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
	{
		shader_now = 4;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
	{
		shader_now = 5;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
	{
		shader_now = 6;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7))
	{
		shader_now = 7;
	}
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

	while (running)
	{
		key_input();
		// show OpenGL version number
		// sf::ContextSettings settings = window.getSettings();
		// std::cout << "OpenGL version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;
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
				Reshape(event.size.width, event.size.height);
			}
		}
		Render();
		window.display();
	}

	return 0;
}