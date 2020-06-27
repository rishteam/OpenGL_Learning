#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

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

#define deg2rad(x) ((x) * ((3.1415926f) / (180.0f)))
#define rad2deg(x) ((180.0f) / ((x) * (3.1415926f)))

//uniform id
struct
{
	GLint  mv_matrix;
	GLint  proj_matrix;
} uniforms;

//model info
typedef struct
{


} Shape;

GLuint          vao;
GLuint          buffer;
GLint           mv_location;
GLint           proj_location;
GLuint			program;			//shader program
mat4			proj_matrix;		//projection matrix
float			aspect;

Shape			m_shape;

//36個點，組出12個三角形
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


GLuint offset = 0;

// Read shader file
std::string LoadShaderSource(const char file[])
{
	std::ifstream ifs(file);
	std::string content;
	if (ifs)
	{
		content.assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())); // read contents
	}
	return content;
}

void ShaderLog(GLuint shader)
{
	GLint isCompiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar *errorLog = new GLchar[maxLength];
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
	static std::string vss = LoadShaderSource("spinning_cube.vs.glsl");
	const char *vsSource = vss.c_str();
	static std::string fss = LoadShaderSource("spinning_cube.fs.glsl");
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


	///////////////////////////
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	mv_location = glGetUniformLocation(program, "mv_matrix");
	proj_location = glGetUniformLocation(program, "proj_matrix");

	//View initial
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, 800, 600);
	float viewportAspect = (float)800 / (float)600;
	//fov, aspect(長寬比例), near, far
	proj_matrix = perspective(deg2rad(45.0f), viewportAspect, 0.1f, 100.0f);
}

float timer = 0, cnt = 0;
#define DELAY 2.0

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update shaders' input variable
	///////////////////////////
	static const GLfloat green[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glUseProgram(program);

	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &proj_matrix[0][0]);
	mat4 Identy_Init(1.0);
	mat4 mv_matrix = translate(Identy_Init, vec3(0.0f, 0.0f, -4.0f));

	sf::Clock clock;
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
	// float currentTime = clock.getElapsedTime().asMilliseconds() * 0.00001f;
	// mat4 mv_matrix = translate(Identy_Init, vec3(0.0f, 0.0f, -4.0f));
	// mv_matrix = translate(mv_matrix, vec3(sinf(2.1f * currentTime) * 0.5f, cosf(1.7f * currentTime) * 0.5f, sinf(1.3f * currentTime) * cosf(1.5f * currentTime) * 2.0f));
	// mv_matrix = rotate(mv_matrix, deg2rad(currentTime * 45.0f), vec3(0.0f, 1.0f, 0.0f));
	// mv_matrix = rotate(mv_matrix, deg2rad(currentTime * 81.0f), vec3(1.0f, 0.0f, 0.0f));
	// glUniformMatrix4fv(mv_location, 1, GL_FALSE, &mv_matrix[0][0]);
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, &mv_matrix[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 36);

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