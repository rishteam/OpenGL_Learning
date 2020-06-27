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

GLuint			square_buffer;
GLuint			square_vao;
GLuint			program;			//shader program
mat4			proj_matrix;		//projection matrix
float			aspect;

Shape			m_shape;

#define deg2rad(x) ((x) * ((3.1415926f) / (180.0f)))
#define rad2deg(x) ((180.0f) / ((x) * (3.1415926f)))

//宣告一個正方形的頂點座標
static const GLfloat square_vertices[] =
{
	-1.0f, -1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f,
	1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 1.0f
};
//宣告正方形的顏色
static const GLfloat instance_colors[] =
{
	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f
};
//宣告每個正方形的所在座標
static const GLfloat instance_positions[] =
{
	-2.0f, -2.0f, 0.0f, 0.0f,
	2.0f, -2.0f, 0.0f, 0.0f,
	2.0f,  2.0f, 0.0f, 0.0f,
	-2.0f,  2.0f, 0.0f, 0.0f
};

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

GLuint offset = 0;

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
	static std::string vss = LoadShaderSource("Instanced_Rendering.vs.glsl");
	const char *vsSource = vss.c_str();
	static std::string fss = LoadShaderSource("Instanced_Rendering.fs.glsl");
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
	//產生VAO
	glGenVertexArrays(1, &square_vao);
	//產生VBO
	glGenBuffers(1, &square_buffer);
	//綁定VAO
	glBindVertexArray(square_vao);
	//綁定VBO
	glBindBuffer(GL_ARRAY_BUFFER, square_buffer);
	//給定VBO大小
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices) + sizeof(instance_colors) + sizeof(instance_positions), NULL, GL_STATIC_DRAW);
	//個別設定存入的參數值，設定頂點座標
	//用glBufferSubData可以透過offset來更新buffer-object中的資料
	offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(square_vertices), square_vertices);
	//移動buffer的offset，使接下來的顏色及座標依序存入
	offset += sizeof(square_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(instance_colors), instance_colors);
	offset += sizeof(instance_colors);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(instance_positions), instance_positions);
	offset += sizeof(instance_positions);


	//指派vertex shader的輸入對應
	//index, size, type, normalized, stride, pointer(指向第一個頂點的第一個屬性位置)
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(square_vertices));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(square_vertices) + sizeof(instance_colors)));

	//啟用頂點屬性陣列
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	//需要的頂點屬性,每隔一個實例更新一次屬性內容
	//update color
	glVertexAttribDivisor(1, 1);
	//update position
	glVertexAttribDivisor(2, 1);
}


void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 0, black);

	glUseProgram(program);
	glBindVertexArray(square_vao);
	//mode, 頂點陣列, 陣列大小, instance次數
	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
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