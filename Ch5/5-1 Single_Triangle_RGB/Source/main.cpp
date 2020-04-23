#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#define MENU_EXIT 3

// using namespace glm;
using namespace std;

GLuint program;			//shader program

//Read shader file
std::string LoadShaderSource(const char file[])
{
	std::ifstream ifs(file);
	std::string content;
	if(ifs)
	{
		content.assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())); // read contents
	}
	return content;
}

//Release 2-dimension array
void FreeShaderSource(char **srcp)
{
	delete srcp[0];
	delete srcp;
}


void ShaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete[] errorLog;
	}
}

void My_Init()
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
	const char *vsSource = LoadShaderSource("vertex.vs.glsl").c_str();
	const char *fsSource = LoadShaderSource("fragment.fs.glsl").c_str();
	glShaderSource(vs, 1, &vsSource, NULL);
	glShaderSource(fs, 1, &fsSource, NULL);
	// FreeShaderSource(vsSource);
	// FreeShaderSource(fsSource);
	glCompileShader(vs);
	glCompileShader(fs);
	ShaderLog(vs);
	ShaderLog(fs);

	//Attach Shader to program
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	// glUseProgram(program);
	///////////////////////////

	float data[18] =
	{
		-0.5f, -0.4f, 0.0f,		//Position
		0.5f, -0.4f, 0.0f,
		0.0f, 0.6f, 0.0f,

		1.0f, 0.0f, 0.0f,		//Color
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	//The type used for names in OpenGL is GLuint
	GLuint buffer;
	//Generate a name dor the buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);


	//產生vao
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//分配屬性資料，座標與顏色的index分別為0,1
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(float) * 9));//offset

	// //啟用頂點屬性陣列，預設為false，因此要打開
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update shaders' input variable
	///////////////////////////

	glDrawArrays(GL_TRIANGLES, 0, 3);
	///////////////////////////
}

//Call to resize the window
// void My_Reshape(int width, int height)
// {
// 	glViewport(0, 0, width, height);
// }

//Timer event
// void My_Timer(int val)
// {
// 	glutPostRedisplay();
// 	glutTimerFunc(16, My_Timer, val);
// }

//Mouse event
// void My_Mouse(int button, int state, int x, int y)
// {
// 	if (button == GLUT_LEFT_BUTTON)
// 	{
// 		if (state == GLUT_DOWN)
// 		{
// 			printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
// 		}
// 		else if (state == GLUT_UP)
// 		{
// 			printf("Mouse %d is released at (%d, %d)\n", button, x, y);
// 		}
// 	}
// 	else if (button == GLUT_RIGHT_BUTTON)
// 	{
// 		printf("Mouse %d is pressed\n", button);
// 	}

// }

//Keyboard event
// void My_Keyboard(unsigned char key, int x, int y)
// {
// 	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
// }

//Special key event
// void My_SpecialKeys(int key, int x, int y)
// {
// 	switch (key)
// 	{
// 	case GLUT_KEY_F1:
// 		printf("F1 is pressed at (%d, %d)\n", x, y);
// 		break;
// 	case GLUT_KEY_PAGE_UP:
// 		printf("Page up is pressed at (%d, %d)\n", x, y);
// 		break;
// 	case GLUT_KEY_LEFT:
// 		printf("Left arrow is pressed at (%d, %d)\n", x, y);
// 		break;
// 	default:
// 		printf("Other special key is pressed at (%d, %d)\n", x, y);
// 		break;
// 	}
// }

//Menu event
// void My_Menu(int id)
// {
// 	switch (id)
// 	{
// 	case MENU_EXIT:
// 		exit(0);
// 		break;
// 	default:
// 		break;
// 	}
// }

// void My_Mouse_Moving(int x, int y) {

// }

int main(int argc, char *argv[])
{
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	// glutInit(&argc, argv);
	// glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// glutInitWindowPosition(100, 100);
	// glutInitWindowSize(600, 600);
	// glutCreateWindow("Framework"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
	// glewInit();

	// //Print debug information
	// DumpInfo();
	// ////////////////////

	// //Call custom initialize function
	// My_Init();

	// //Define Menu
	// ////////////////////
	// int menu_main = glutCreateMenu(My_Menu);
	// int menu_entry = glutCreateMenu(My_Menu);

	// glutSetMenu(menu_main);
	// glutAddMenuEntry("Exit", MENU_EXIT);

	// glutSetMenu(menu_main);
	// glutAttachMenu(GLUT_RIGHT_BUTTON);
	// ////////////////////

	// //Register GLUT callback functions
	// ////////////////////
	// glutDisplayFunc(My_Display);
	// glutReshapeFunc(My_Reshape);
	// glutMouseFunc(My_Mouse);
	// glutKeyboardFunc(My_Keyboard);
	// glutSpecialFunc(My_SpecialKeys);
	// glutTimerFunc(16, My_Timer, 0);
	// glutPassiveMotionFunc(My_Mouse_Moving);
	// glutMotionFunc(My_Mouse_Moving);
	// ////////////////////

	// // Enter main event loop.
	// glutMainLoop();

	sf::Window window(sf::VideoMode(800, 600), "OpenGL");
	window.setVerticalSyncEnabled(true);
	//check close
	bool running = true;
	// activate the window
	window.setActive(true);
	//Call custom initialize function
	My_Init();

	while (running)
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				// end the program
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				// adjust the viewport when the window is resized
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}
		//Register GLUT callback functions
		My_Display();
		window.display();
	}
	return 0;
}