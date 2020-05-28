#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

#include "shader_compile.h"
#include "ViewManager.h"

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

//uniform id
struct
{
	GLint  mv_matrix;
	GLint  proj_matrix;
} uniforms;

GLubyte timer_cnt = 0;
unsigned int timer_speed = 16;

ViewManager m_camara;

GLuint vao;
GLuint          buffer;
GLint           mv_location;
GLint           proj_location;

GLuint			program;			//shader program
GLuint			program2;
mat4			proj_matrix;		//projection matrix

GLuint          window_vao;
GLuint			window_buffer;
GLuint			FBO;
GLuint			depthRBO;
GLuint			FBODataTexture;


static const GLfloat vertex_positions[] =
{
	-0.25f,  0.25f, -0.25f,
	-0.25f, -0.25f, -0.25f,
	0.25f, -0.25f, -0.25f,

	0.25f, -0.25f, -0.25f,
	0.25f,  0.25f, -0.25f,
	-0.25f,  0.25f, -0.25f,

	0.25f, -0.25f, -0.25f,
	0.25f, -0.25f,  0.25f,
	0.25f,  0.25f, -0.25f,

	0.25f, -0.25f,  0.25f,
	0.25f,  0.25f,  0.25f,
	0.25f,  0.25f, -0.25f,

	0.25f, -0.25f,  0.25f,
	-0.25f, -0.25f,  0.25f,
	0.25f,  0.25f,  0.25f,

	-0.25f, -0.25f,  0.25f,
	-0.25f,  0.25f,  0.25f,
	0.25f,  0.25f,  0.25f,

	-0.25f, -0.25f,  0.25f,
	-0.25f, -0.25f, -0.25f,
	-0.25f,  0.25f,  0.25f,

	-0.25f, -0.25f, -0.25f,
	-0.25f,  0.25f, -0.25f,
	-0.25f,  0.25f,  0.25f,

	-0.25f, -0.25f,  0.25f,
	0.25f, -0.25f,  0.25f,
	0.25f, -0.25f, -0.25f,

	0.25f, -0.25f, -0.25f,
	-0.25f, -0.25f, -0.25f,
	-0.25f, -0.25f,  0.25f,

	-0.25f,  0.25f, -0.25f,
	0.25f,  0.25f, -0.25f,
	0.25f,  0.25f,  0.25f,

	0.25f,  0.25f,  0.25f,
	-0.25f,  0.25f,  0.25f,
	-0.25f,  0.25f, -0.25f
};

static const GLfloat window_positions[] =
{
	1.0f,-1.0f,1.0f,0.0f,
	-1.0f,-1.0f,0.0f,0.0f,
	-1.0f,1.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f
};

void init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glewInit();

	//Initialize shader
	///////////////////////////
	program = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	static std::string vss = LoadShaderSource("./spinning_cube.vs.glsl");
	const char *vsSource = vss.c_str();
	static std::string fss = LoadShaderSource("./spinning_cube.fs.glsl");
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

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	mv_location = glGetUniformLocation(program, "mv_matrix");
	proj_location = glGetUniformLocation(program, "proj_matrix");


	///////////////////////////
	//Initialize shader2
	///////////////////////////
	program2 = glCreateProgram();

	GLuint vs2 = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs2 = glCreateShader(GL_FRAGMENT_SHADER);
	static std::string vss2 = LoadShaderSource("./gray.vs.glsl");
	const char *vsSource2 = vss2.c_str();
	static std::string fss2 = LoadShaderSource("./gray.fs.glsl");
	const char *fsSource2 = fss2.c_str();
	glShaderSource(vs2, 1, &vsSource2, NULL);
	glShaderSource(fs2, 1, &fsSource2, NULL);
	glCompileShader(vs2);
	glCompileShader(fs2);
	ShaderLog(vs2);
	ShaderLog(fs2);

	//Attach Shader to program
	glAttachShader(program2, vs2);
	glAttachShader(program2, fs2);
	glLinkProgram(program2);

	glGenVertexArrays(1, &window_vao);
	glBindVertexArray(window_vao);

	glGenBuffers(1, &window_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, window_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(window_positions), window_positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (const GLvoid*)(sizeof(GL_FLOAT) * 2));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//create FBO
	glGenFramebuffers(1, &FBO);

	//view position
	// glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// glViewport(0, 0, 800, 600);
	// float viewportAspect = (float)800 / (float)600;
	//fov, aspect(長寬比例), near, far
	// proj_matrix = perspective(deg2rad(45.0f), viewportAspect, 0.1f, 100.0f);
}

float timer = 0, cnt = 0;
#define DELAY 2.0
void Render()
{
	//將以下東西綁到FBO上
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Update cube shader
		///////////////////////////
		static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		glClearBufferfv(GL_COLOR, 0, green);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glUseProgram(program);

		glBindVertexArray(vao);
		glUniformMatrix4fv(proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

		mat4 Identy_Init(1.0);
		// auto currentTime = clock.getElapsedTimeglGetUniformLocation().asMicroseconds();
		// float currentTime = 0.005f;
		// float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
		mat4 mv_matrix = translate(Identy_Init, vec3(0.0f, 0.0f, -4.0f));
		// mv_matrix = translate(mv_matrix, vec3(sinf(2.1f * currentTime) * 0.5f, cosf(1.7f * currentTime) * 0.5f, sinf(1.3f * currentTime) * cosf(1.5f * currentTime) * 2.0f));

		sf::Clock clock; // starts the clock
		float unit_time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		timer += unit_time;

		if (timer > DELAY)
		{
			if (cnt == 360)
				cnt = 0;
			else
				cnt += 45;

			timer = 0;
		}
		mv_matrix = rotate(mv_matrix, deg2rad(cnt), vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(mv_location, 1, GL_FALSE, &mv_matrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//結束點

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBODataTexture);
	glBindVertexArray(window_vao);
	glUseProgram(program2);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

}

//Reshape後要重新設定
void Reshape(int width, int height)
{

	float aspect = width * 1.0f / height;
	m_camara.SetWindowSize(width, height);
	glViewport(0, 0, width, height);
	// float viewportAspect = (float)width / (float)height;
	// m_camara.SetWindowSize(width, height);
	// proj_matrix = perspective(deg2rad(50.0f), viewportAspect, 0.1f, 1000.0f);
	// m_camara.GetModelMatrix();
	// m_camara.GetViewMatrix();
	// m_camara.GetProjectionMatrix(viewportAspect);
	m_camara.GetModelViewProjectionMatrix(aspect);
	glDeleteRenderbuffers(1, &depthRBO);
	glDeleteTextures(1, &FBODataTexture);
	glGenRenderbuffers(1, &depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

	glGenTextures(1, &FBODataTexture);
	glBindTexture(GL_TEXTURE_2D, FBODataTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBODataTexture, 0);
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