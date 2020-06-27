#include "../../Include/Common.h"
#include "../../Include/ViewManager.h"

#define MENU_Entry1 1
#define MENU_Entry2 2
#define MENU_EXIT   3
#define TESS
using namespace glm;
using namespace std;

//uniform id
struct
{
	GLint  mv_matrix;
	GLint  proj_matrix;
	GLint  normal_matrix;
	GLint  B;
	GLint  BT;

	GLint  zoom;

	GLint  TessLevelInner;
	GLint  TessLevelOuter;
} uniforms;

//model info
typedef struct
{
	GLuint vao;
	GLuint vbo;
	GLuint vboTex;
	GLuint ebo;

	GLuint p_normal;
	int materialId;
	int indexCount;
	GLuint m_texture;

	mat4 model;
} Shape;

GLuint			program;			//shader program
mat4			proj_matrix;		//projection matrix
float			aspect;

Shape			m_shape;			
ViewManager		m_camera;

float TessLevelInner = 3;
float TessLevelOuter = 3;

void My_LoadModels()
{
	//Load Model
	/////////////////////////////
	const int Faces[] = {
		0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

	const float Verts[] = {
		-1.500f,  -1.500f,  4.000f,
		-0.500f,  -1.500f,  4.000f,
		0.500f,   -1.500f,  1.000f,
		1.500f,   -1.500f,  1.000f,
		-1.500f,  -0.500f,  1.000f,
		-0.500f,  -0.500f,  3.000f,
		0.500f,   -0.500f,  3.000f,
		1.500f,   -0.500f,  1.000f,
		-1.500f,  0.500f,  1.000f,
		-0.500f,  0.500f,  4.000f,
		0.500f,   0.500f,  4.000f,
		1.500f,   0.500f,  2.000f,
		-1.500f,  1.500f,  1.000f,
		-0.500f,  1.500f,  1.000f,
		0.500f,   1.500f,  1.000f,
		1.500f,   1.500f,  1.000f
		
	};
	///////////////////////////	
	//Bind model data
	///////////////////////////
	glGenVertexArrays(1, &m_shape.vao);
	glBindVertexArray(m_shape.vao);

	glGenBuffers(3, &m_shape.vbo);
	glGenBuffers(1, &m_shape.p_normal);
	glBindBuffer(GL_ARRAY_BUFFER, m_shape.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_shape.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);

	m_shape.indexCount = sizeof(Faces) / sizeof(Faces[0]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	///////////////////////////

	//Texture setting
	///////////////////////////	
	//Load texture data from file
	

	//Generate empty texture
	
	
	//Do texture setting
	
	///////////////////////////	
}

void My_Init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Initialize shaders
	///////////////////////////	
    program = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLuint tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	char** vsSource = LoadShaderSource("Example3Shader/bezier_surface.vs.glsl");
	char** tcsSource = LoadShaderSource("Example3Shader/bezier_surface.tcs.glsl");
	char** tesSource = LoadShaderSource("Example3Shader/bezier_surface.tes.glsl");
	char** gsSource = LoadShaderSource("Example3Shader/bezier_surface.gs.glsl");
	char** fsSource = LoadShaderSource("Example3Shader/bezier_surface.fs.glsl");
	glShaderSource(vs, 1, vsSource, NULL);
	glShaderSource(tcs, 1, tcsSource, NULL);
	glShaderSource(tes, 1, tesSource, NULL);
	glShaderSource(gs, 1, gsSource, NULL);
	glShaderSource(fs, 1, fsSource, NULL);
	FreeShaderSource(vsSource);
	FreeShaderSource(tcsSource);
	FreeShaderSource(tesSource);
	FreeShaderSource(gsSource);
	FreeShaderSource(fsSource);
	glCompileShader(vs);
	glCompileShader(tcs);
	glCompileShader(tes);
	glCompileShader(gs);
	glCompileShader(fs);

	printf("VERTEX_SHADER\n");
	ShaderLog(vs);
	printf("GL_TESS_CONTROL_SHADER\n");
	ShaderLog(tcs);
	printf("GL_TESS_EVALUATION_SHADER\n");
	ShaderLog(tes);
	printf("GL_GEOMETRY_SHADER\n");
	ShaderLog(gs);
	printf("GL_FRAGMENT_SHADER\n");
	ShaderLog(fs);   
    
	//Attach Shader to program
	glAttachShader(program, vs);
	glAttachShader(program, tcs);
	glAttachShader(program, tes);
	glAttachShader(program, gs);
    glAttachShader(program, fs);
    glLinkProgram(program);

	//Cache uniform variable id
	uniforms.proj_matrix = glGetUniformLocation(program, "um4p");
	uniforms.mv_matrix = glGetUniformLocation(program, "um4mv");
	uniforms.normal_matrix = glGetUniformLocation(program, "um3n");
	uniforms.zoom = glGetUniformLocation(program, "uf1zoom");
	uniforms.TessLevelInner = glGetUniformLocation(program, "TessLevelInner");
	uniforms.TessLevelOuter = glGetUniformLocation(program, "TessLevelOuter");
	uniforms.B = glGetUniformLocation(program, "B");
	uniforms.BT = glGetUniformLocation(program, "BT");
	glUseProgram(program);
	///////////////////////////	

	//Load model to shader program
	My_LoadModels();
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	//Update shaders' input variable
	///////////////////////////	
	glBindVertexArray(m_shape.vao);
	glUseProgram(program);

	GLfloat move = glutGet(GLUT_ELAPSED_TIME) / 50.0;
	m_shape.model = mat4();

	// tell OpenGL that every patch has 16 verts
	glPatchParameteri(GL_PATCH_VERTICES, 16);

	glUniformMatrix4fv(uniforms.B, 1, GL_FALSE, value_ptr(glm::mat4(glm::vec4(-1, 3, -3, 1),
		glm::vec4(3, -6, 3, 0),
		glm::vec4(-3, 3, 0, 0),
		glm::vec4(1, 0, 0, 0)
		)));
	glUniformMatrix4fv(uniforms.BT, 1, GL_TRUE, value_ptr(glm::mat4(glm::vec4(-1, 3, -3, 1),
		glm::vec4(3, -6, 3, 0),
		glm::vec4(-3, 3, 0, 0),
		glm::vec4(1, 0, 0, 0)
		)));
	glUniform1f(uniforms.TessLevelInner, TessLevelInner);
	glUniform1f(uniforms.TessLevelOuter, TessLevelOuter);
	glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, value_ptr(m_camera.GetViewMatrix() * m_camera.GetModelMatrix() * m_shape.model));
	glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, value_ptr(m_camera.GetProjectionMatrix(aspect)));
	glUniformMatrix3fv(uniforms.normal_matrix, 1, GL_FALSE, value_ptr(glm::mat3(glm::inverse(glm::transpose(m_camera.GetProjectionMatrix(aspect))))));

	
	glDrawElements(GL_PATCHES, m_shape.indexCount, GL_UNSIGNED_INT, 0);
	///////////////////////////	


    glutSwapBuffers();


}

//Call to resize the window
void My_Reshape(int width, int height)
{
	aspect = width * 1.0f / height;
	m_camera.SetWindowSize(width, height);
	glViewport(0, 0, width, height);
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
	m_camera.keyEvents(key);

	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
}

//Special key event
void My_SpecialKeys(int key, int x, int y)
{
	
	switch(key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		TessLevelInner--;
		if (TessLevelInner < 3) TessLevelInner = 3;
		printf("TessLevelInner = %.0f\n", TessLevelInner);
		break;
	case GLUT_KEY_RIGHT:
		TessLevelInner++;
		printf("TessLevelInner = %.0f\n", TessLevelInner);
		break;
	case GLUT_KEY_UP:
		TessLevelOuter++;
		printf("TessLevelOuter = %.0f\n", TessLevelOuter);
		break;
	case GLUT_KEY_DOWN:
		TessLevelOuter--;
		if (TessLevelOuter < 3) TessLevelOuter = 3;
		printf("TessLevelOuter = %.0f\n", TessLevelOuter);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

//Menu event
void My_Menu(int id)
{
	switch(id)
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
	//int menu_main = glutCreateMenu(My_Menu);
	//int menu_entry = glutCreateMenu(My_Menu);

	//glutSetMenu(menu_main);
	//glutAddSubMenu("Entry", menu_entry);
	//glutAddMenuEntry("Exit", MENU_EXIT);

	//glutSetMenu(menu_entry);
	//glutAddMenuEntry("Print Entry1", MENU_Entry1);
	//glutAddMenuEntry("Print Entry2", MENU_Entry2);

	//glutSetMenu(menu_main);
	//glutAttachMenu(GLUT_RIGHT_BUTTON);
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