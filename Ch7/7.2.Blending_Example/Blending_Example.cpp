#include "../../Include/Common.h"

#define src_ZERO 0
#define src_ONE 1
#define src_SC 2
#define src_OMSC 3
#define src_SA 4
#define src_OMSA 5
#define src_DC 6
#define src_OMDC 7
#define src_DA 8
#define src_OMDA 9

#define dst_ZERO 10
#define dst_ONE 11
#define dst_SC 12
#define dst_OMSC 13
#define dst_SA 14
#define dst_OMSA 15
#define dst_DC 16
#define dst_OMDC 17
#define dst_DA 18
#define dst_OMDA 19

#define MENU_EXIT  20

using namespace glm;
using namespace std;

GLuint			program;			//shader program

GLuint wood_texture;
GLuint light_texture;
int defalut_w = 800;
int defalut_h = 800;

int sfactor = 1;
int dfactor = 1;

string sfactor_s = "GL_ONE";
string dfactor_s = "GL_ONE";

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
	char** vsSource = LoadShaderSource("./7.2.Blending_Example/Blending_Example.vs.glsl");
	char** fsSource = LoadShaderSource("./7.2.Blending_Example/Blending_Example.fs.glsl");
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

	TextureData tdata = Load_png("./7.2.Blending_Example/wood.png");

	glGenTextures(1, &wood_texture);
	glBindTexture(GL_TEXTURE_2D, wood_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	TextureData tdata1 = Load_png("./7.2.Blending_Example/light.png");

	glGenTextures(1, &light_texture);
	glBindTexture(GL_TEXTURE_2D, light_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata1.width, tdata1.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata1.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	///////////////////////////////////////////////////////////////////////////
	printf("\nNote : Use Right Click Menu to switch Effect\n");
	//////////////////////////////////////////////////////////////////////////

	printf("\nsfactor: %s  dfactor: %s\n",sfactor_s.c_str(),dfactor_s.c_str());
}

// GLUT callback. Called to draw the scene.
void My_Display()
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
	///////////////////////////	

	glutSwapBuffers();
}

//Call to resize the window
void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glutPostRedisplay();
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
	case src_ZERO:
		sfactor_s = "GL_ZERO";
		sfactor = GL_ZERO;
		break;
	case src_ONE:
		sfactor_s = "GL_ONE";
		sfactor = GL_ONE;
		break;
	case src_SC:
		sfactor_s = "GL_SRC_COLOR";
		sfactor = GL_SRC_COLOR;
		break;
	case src_OMSC:
		sfactor_s = "GL_ONE_MINUS_SRC_COLOR";
		sfactor = GL_ONE_MINUS_SRC_COLOR;
		break;
	case src_SA:
		sfactor_s = "GL_SRC_ALPHA";
		sfactor = GL_SRC_ALPHA;
		break;
	case src_OMSA:
		sfactor_s = "GL_ONE_MINUS_SRC_ALPHA";
		sfactor = GL_ONE_MINUS_SRC_ALPHA;
		break;
	case src_DC:
		sfactor_s = "GL_DST_COLOR";
		sfactor = GL_DST_COLOR;
		break;
	case src_OMDC:
		sfactor_s = "GL_ONE_MINUS_DST_COLOR";
		sfactor = GL_ONE_MINUS_DST_COLOR;
		break;
	case src_DA:
		sfactor_s = "GL_DST_ALPHA";
		sfactor = GL_DST_ALPHA;
		break;
	case src_OMDA:
		sfactor_s = "GL_ONE_MINUS_DST_ALPHA";
		sfactor = GL_ONE_MINUS_DST_ALPHA;
		break;
	case dst_ZERO:
		dfactor_s = "GL_ZERO";
		dfactor = GL_ZERO;
		break;
	case dst_ONE:
		dfactor_s = "GL_ONE";
		dfactor = GL_ONE;
		break;
	case dst_SC:
		dfactor_s = "GL_ZGL_SRC_COLORERO";
		dfactor = GL_SRC_COLOR;
		break;
	case dst_OMSC:
		dfactor_s = "GL_ONE_MINUS_SRC_COLOR";
		dfactor = GL_ONE_MINUS_SRC_COLOR;
		break;
	case dst_SA:
		dfactor_s = "GL_SRC_ALPHA";
		dfactor = GL_SRC_ALPHA;
		break;
	case dst_OMSA:
		dfactor_s = "GL_ONE_MINUS_SRC_ALPHA";
		dfactor = GL_ONE_MINUS_SRC_ALPHA;
		break;
	case dst_DC:
		dfactor_s = "GL_DST_COLOR";
		dfactor = GL_DST_COLOR;
		break;
	case dst_OMDC:
		dfactor_s = "GL_ONE_MINUS_DST_COLOR";
		dfactor = GL_ONE_MINUS_DST_COLOR;
		break;
	case dst_DA:
		dfactor_s = "GL_DST_ALPHA";
		dfactor = GL_DST_ALPHA;
		break;
	case dst_OMDA:
		dfactor_s = "GL_ONE_MINUS_DST_ALPHA";
		dfactor = GL_ONE_MINUS_DST_ALPHA;
		break;
	default:
		break;
	}

	printf("\nsfactor: %s  dfactor: %s\n", sfactor_s.c_str(), dfactor_s.c_str());

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
	int sfactor = glutCreateMenu(My_Menu);
	int dfactor = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("source factor", sfactor);
	glutAddSubMenu("destination factor", dfactor);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(sfactor);
	glutAddMenuEntry("GL_ZERO", src_ZERO);
	glutAddMenuEntry("GL_ONE", src_ONE);
	glutAddMenuEntry("GL_SRC_COLOR", src_SC);
	glutAddMenuEntry("GL_ONE_MINUS_SRC_COLOR", src_OMSC);
	glutAddMenuEntry("GL_SRC_ALPHA", src_SA);
	glutAddMenuEntry("GL_ONE_MINUS_SRC_ALPHA", src_OMSA);
	glutAddMenuEntry("GL_DST_COLOR", src_DC);
	glutAddMenuEntry("GL_ONE_MINUS_DST_COLOR", src_OMDC);
	glutAddMenuEntry("GL_DST_ALPHA", src_DA);
	glutAddMenuEntry("GL_ONE_MINUS_DST_ALPHA", src_OMDA);

	glutSetMenu(dfactor);
	glutAddMenuEntry("GL_ZERO", dst_ZERO);
	glutAddMenuEntry("GL_ONE", dst_ONE);
	glutAddMenuEntry("GL_SRC_COLOR", dst_SC);
	glutAddMenuEntry("GL_ONE_MINUS_SRC_COLOR", dst_OMSC);
	glutAddMenuEntry("GL_SRC_ALPHA", dst_SA);
	glutAddMenuEntry("GL_ONE_MINUS_SRC_ALPHA", dst_OMSA);
	glutAddMenuEntry("GL_DST_COLOR", dst_DC);
	glutAddMenuEntry("GL_ONE_MINUS_DST_COLOR", dst_OMDC);
	glutAddMenuEntry("GL_DST_ALPHA", dst_DA);
	glutAddMenuEntry("GL_ONE_MINUS_DST_ALPHA", dst_OMDA);


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