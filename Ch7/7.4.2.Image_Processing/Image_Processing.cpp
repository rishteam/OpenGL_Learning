#include "../../Include/Common.h"

#define Normal 0
#define Shader_Average_filter 1
#define Shader_Median_filter 2
#define Shader_Triangle_filter 3
#define Shader_Gaussian_filter 4
#define Shader_Quantization 5
#define Shader_DoG 6
#define Shader_Image_Abstraction 7
#define MENU_EXIT   8

using namespace glm;
using namespace std;

GLuint			program;			//shader program

int shader_now = 0;

GLuint hawk_texture;
GLint Shader_now_Loc;
int defalut_w = 1006;
int defalut_h = 600;

void My_Init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Initialize shaders
	///////////////////////////	
	program = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	char** vsSource = LoadShaderSource("./7.4.2.Image_Processing/Image_Processing.vs.glsl");
	char** fsSource = LoadShaderSource("./7.4.2.Image_Processing/Image_Processing.fs.glsl");
	glShaderSource(vs, 1, vsSource, NULL);
	glShaderSource(fs, 1, fsSource, NULL);
	//FreeShaderSource(vsSource);
	//FreeShaderSource(fsSource);
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

	TextureData tdata = Load_png("./7.4.2.Image_Processing/hawk.png");

	glGenTextures(1, &hawk_texture);
	glBindTexture(GL_TEXTURE_2D, hawk_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	///////////////////////////////////////////////////////////////////////////
	printf("\nNote : Use Right Click Menu to switch Effect\n");
	//////////////////////////////////////////////////////////////////////////
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update shaders' input variable
	///////////////////////////	
	glUseProgram(program);
	glUniform1i(Shader_now_Loc, shader_now);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	///////////////////////////	

	glutSwapBuffers();
}

//Call to resize the window
void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

//Timer event
void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(16, My_Timer, val);
}

//Menu event
void My_Menu(int id)
{
	switch (id)
	{
	case MENU_EXIT:
		exit(0);
		break;
	case Normal:
		shader_now = 0;
		break;
	case Shader_Average_filter:
		shader_now = 1;
		break;
	case Shader_Median_filter:
		shader_now = 2;
		break;
	case Shader_Triangle_filter:
		shader_now = 3;
		break;
	case Shader_Gaussian_filter:
		shader_now = 4;
		break;
	case Shader_Quantization:
		shader_now = 5;
		break;
	case Shader_DoG:
		shader_now = 6;
		break;
	case Shader_Image_Abstraction:
		shader_now = 7;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(defalut_w, defalut_h);
	glutCreateWindow("Image_Processing"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif

	//Print debug information 
	DumpInfo();
	////////////////////

	//Call custom initialize function
	My_Init();

	//Define Menu
	////////////////////
	int menu_main = glutCreateMenu(My_Menu);
	int shader = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Shader", shader);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(shader);
	glutAddMenuEntry("Normal", Normal);
	glutAddMenuEntry("Average filter", Shader_Average_filter);
	glutAddMenuEntry("Median filter", Shader_Median_filter);
	glutAddMenuEntry("Triangle filter", Shader_Triangle_filter);
	glutAddMenuEntry("Gaussian filter", Shader_Gaussian_filter);
	glutAddMenuEntry("Quantization", Shader_Quantization);
	glutAddMenuEntry("DoG", Shader_DoG);
	glutAddMenuEntry("Image Abstraction", Shader_Image_Abstraction);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	////////////////////

	//Register GLUT callback functions
	////////////////////
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutTimerFunc(16, My_Timer, 0);
	////////////////////

	// Enter main event loop.
	glutMainLoop();

	return 0;
}