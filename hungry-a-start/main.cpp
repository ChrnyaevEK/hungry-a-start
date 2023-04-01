#include <iostream>
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>

#include "view.h"
#include "controller.h"

void handleGameLose(Controller& c);
void handleGameWin(Controller& c);

void handleGameLose(Controller& c) {
	c.hookSetup();
};
void handleGameWin(Controller& c) {
	c.hookSetup();
};

int main()
{
	const int keyCooldownMs = 10;
	sf::RenderWindow window(sf::VideoMode(1000, 1000), GameTitle);
	//window.setKeyRepeatEnabled(false)

	ControllerMain* cMain = new ControllerMain();
	cMain->registerEvent(cMain->eGameLose, handleGameLose);
	cMain->registerEvent(cMain->eGameWin, handleGameWin);

	cMain->hookSetup();
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}


		// Handle control
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			cMain->hookArrowLeft();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			cMain->hookArrowRight();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			cMain->hookArrowUp();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			cMain->hookArrowDown();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			cMain->hookReturnKey();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			cMain->hookEscKey();
		}

		cMain->render(&window);

		cMain->hookUpdateState();
		std::this_thread::sleep_for(std::chrono::milliseconds(keyCooldownMs));
	}

	return 0;
}