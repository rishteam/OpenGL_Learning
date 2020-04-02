#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

using namespace std;

void My_Display()
{
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
	sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL");
	window.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(window);
	sf::Clock deltaClock;
	//check close
	bool running = true;
	// activate the window
	window.setActive(true);
	// handle events
	sf::Event event;

	sf::CircleShape cir(50.f);
	cir.setPosition(100, 100);
	cir.setFillColor(sf::Color::Red);

	while (running)
	{
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
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
		ImGui::SFML::Update(window, deltaClock.restart());
		// ImGui Update
		ImGui::Begin("Debug");
		ImGui::End();
		/////////////////////////////////////////////////////////////////
		// Draw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		My_Display();
		// SFML draws
		window.pushGLStates();
			window.draw(cir);
		window.popGLStates();
		// ImGui draws
		window.pushGLStates();
			ImGui::SFML::Render(window);
		window.popGLStates();
		window.display();
	}
	ImGui::SFML::Shutdown();
}