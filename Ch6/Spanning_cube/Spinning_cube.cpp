#include "../../Include/Common.h"
#include "../../Include/ViewManager.h"

#define MENU_Entry1 1
#define MENU_Entry2 2
#define MENU_EXIT   3

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

GLuint          vao;
GLuint          buffer;
GLint           mv_location;
GLint           proj_location;
GLuint			program;			//shader program
mat4			proj_matrix;		//projection matrix
float			aspect;

Shape			m_shape;
ViewManager		m_camera;

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
	char** vsSource = LoadShaderSource("./Spanning_cube/spinning_cube.vs.glsl");
	char** fsSource = LoadShaderSource("./Spanning_cube/spinning_cube.fs.glsl");
	glShaderSource(vs, 1, vsSource, NULL);
	glShaderSource(fs, 1, fsSource, NULL);
	FreeShaderSource(vsSource);
	FreeShaderSource(fsSource);
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
	//Load model to shader program

}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update shaders' input variable
	///////////////////////////	
	static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glUseProgram(program);

	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

	mat4 Identy_Init(1.0);
	float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	mat4 mv_matrix = translate(Identy_Init, vec3(0.0f, 0.0f, -4.0f));
	mv_matrix = translate(mv_matrix, vec3(sinf(2.1f * currentTime) * 0.5f, cosf(1.7f * currentTime) * 0.5f, sinf(1.3f * currentTime) * cosf(1.5f * currentTime) * 2.0f));
	mv_matrix = rotate(mv_matrix, deg2rad(currentTime * 45.0f), vec3(0.0f, 1.0f, 0.0f));
	mv_matrix = rotate(mv_matrix, deg2rad(currentTime * 81.0f), vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, &mv_matrix[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	///////////////////////////	

	glutSwapBuffers();
}

//Call to resize the window
void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	proj_matrix = perspective(deg2rad(50.0f), viewportAspect, 0.1f, 100.0f);
}

//Timer event
void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(16, My_Timer, val);
}

//Mouse event
void My_Mouse(int button, int state, int x, int y)
{
	m_camera.mouseEvents(button, state, x, y);

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		}
		else if (state == GLUT_UP)
		{
			printf("Mouse %d is released at (%d, %d)\n", button, x, y);
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		printf("Mouse %d is pressed\n", button);
	}

}

//Keyboard event
void My_Keyboard(unsigned char key, int x, int y)
{
	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
}

//Special key event
void My_SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

//Menu event
void My_Menu(int id)
{
	switch (id)
	{
	case MENU_Entry1:
		printf("Entry1 is selected.\n");
		break;
	case MENU_Entry2:
		printf("Entry2 is selected.\n");
		break;
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
}

void My_Mouse_Moving(int x, int y) {
	m_camera.mouseMoveEvent(x, y);
}

int main(int argc, char *argv[])
{
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Framework"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
	glewInit();

	//Print debug information 
	DumpInfo();
	////////////////////

	//Call custom initialize function
	My_Init();

	//Define Menu
	////////////////////
	int menu_main = glutCreateMenu(My_Menu);
	int menu_entry = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Entry", menu_entry);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_entry);
	glutAddMenuEntry("Print Entry1", MENU_Entry1);
	glutAddMenuEntry("Print Entry2", MENU_Entry2);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	////////////////////

	//Register GLUT callback functions
	////////////////////
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(16, My_Timer, 0);
	glutPassiveMotionFunc(My_Mouse_Moving);
	glutMotionFunc(My_Mouse_Moving);
	////////////////////

	// Enter main event loop.
	glutMainLoop();

	return 0;
}