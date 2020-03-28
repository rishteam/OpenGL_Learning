#include "../../Include/Common.h"

using namespace std;

GLuint			triangle_buffer;
GLuint			triangle_vao;
GLuint			program;

static const GLfloat triangle_vertices[] =
{
	-0.4f, -0.4f, 0.0f, 1.0f,
	 0.0f,  0.4f, 0.0f, 1.0f,
	 0.4f, -0.4f, 0.0f, 1.0f,
};

char* vsSource = R"glsl(
	#version 400

	layout (location = 0) in vec4 position;

	void main()
	{
		gl_Position = position;
	}
)glsl";
char* fsSource = R"glsl(
	#version 400

	out vec4 color;

	void main()
	{
		color = vec4(0.0, 1.0, 0.0, 1.0);
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
	glGenVertexArrays(1, &triangle_vao);
	glBindVertexArray(triangle_vao);
	glGenBuffers(1, &triangle_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, triangle_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices) * sizeof(float), &triangle_vertices[0], GL_STATIC_DRAW);
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
		glBindVertexArray(triangle_vao);
		//每三個頂點之間畫三角形，從buffer的哪個位置開始畫，頂點數量
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	///////////////////////////	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("4.2.Shader"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
	//Call custom initialize function
	My_Init();
	//Register GLUT callback functions
	glutDisplayFunc(My_Display);

	// Enter main event loop.
	glutMainLoop();

	return 0;
}