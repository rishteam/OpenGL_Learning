#include "../../Include/Common.h"

using namespace std;

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update shaders' input variable
	///////////////////////////
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
		glVertex2f(-0.4, -0.4);
		glVertex2f( 0.0,  0.4);
		glVertex2f( 0.4, -0.4);
	glEnd();

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
	glutCreateWindow("4.1.Basic"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//Register GLUT callback functions
	////////////////////
	glutDisplayFunc(My_Display);
	////////////////////

	// Enter main event loop.
	glutMainLoop();

	return 0;
}