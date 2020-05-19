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

GLuint wood_texture;
GLuint light_texture;
mat4 proj_matrix;

int defalut_w = 800;
int defalut_h = 800;

int sfactor = 1;
int dfactor = 1;

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
	static std::string vss = LoadShaderSource("./Blending_Example.vs.glsl");
	const char *vsSource = vss.c_str();
	static std::string fss = LoadShaderSource("./Blending_Example.fs.glsl");
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

	TextureData tdata = Load_png("./wood.png");

	glGenTextures(1, &wood_texture);
	glBindTexture(GL_TEXTURE_2D, wood_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	TextureData tdata1 = Load_png("./light.png");

	glGenTextures(1, &light_texture);
	glBindTexture(GL_TEXTURE_2D, light_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata1.width, tdata1.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata1.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

// GLUT callback. Called to draw the scene.
void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);

	//Update shaders' input variable
	///////////////////////////	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(sfactor, dfactor);

	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wood_texture);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindTexture(GL_TEXTURE_2D, light_texture);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glUseProgram(0);
}

int main(int argc, char *argv[])
{
	/* SRC:
	  GL_ZERO                   將來源顏色設為0,0,0,0
	  GL_ONE                    用原本的顏色(原本的貼圖)
	  GL_DST_COLOR              = 來源顏色×背景顏色
	  GL_ONE_MINUS_DST_COLOR    = 來源顏色×(1,1,1,1-背景顏色)
	  GL_SRC_ALPHA              = 來源顏色×自己的Alpha值
	  GL_ONE_MINUS_SRC_ALPHA    = 來源顏色×(1-自己的Alpha值)
	*/
	/* DST:
	GL_ZERO                   將背景顏色設為0,0,0,0
	GL_ONE                    用背景的顏色
	GL_SRC_COLOR              = 來源顏色×背景顏色
	GL_ONE_MINUS_SRC_COLOR    = 目標顏色×(1,1,1,1-來源顏色)
	GL_SRC_ALPHA              = 來源顏色×(來源顏色的Alpha值)
	GL_ONE_MINUS_SRC_ALPHA    = 來源顏色×(1-來源顏色的Alpha值)
	*/
	sfactor = GL_SRC_ALPHA;
	dfactor = GL_SRC_ALPHA;

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
				glViewport(0, 0, event.size.width, event.size.height);
				float viewportAspect = (float)event.size.width / (float)event.size.height;
				proj_matrix = perspective(deg2rad(30.0f), viewportAspect, 0.1f, 100.0f);
			}
		}
		Render();
		window.display();
	}

	return 0;
}