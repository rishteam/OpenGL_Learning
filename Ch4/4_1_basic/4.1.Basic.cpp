#include "../../Include/Common.h"
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

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
}

int main(int argc, char *argv[])
{
	sf::Window window(sf::VideoMode(800, 600), "OpenGL");
	window.setVerticalSyncEnabled(true);
	//check close
	bool running = true;
	// activate the window
	window.setActive(true);
	// handle events
	sf::Event event;

	while (running)
	{
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
		My_Display();
		window.display();
	}

	return 0;
}