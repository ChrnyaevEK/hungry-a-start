#include<iostream>
#include<cstdlib>
#include <chrono>
#include "controller.h"
#include "solver_a_star.h"

void Controller::render(sf::RenderWindow* window)
{
	window->clear();
	this->draw(window);
	window->display();
}

bool Controller::randomBool() {
	//srand((unsigned)(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
	return 0 + (rand() % (1 - 0 + 1)) == 1;
}

int Controller::randomInt(int mx) {
	//srand((unsigned)(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
	return rand() % mx + 1;
}

void Controller::registerEvent(std::string event, void (*handler)(Controller& c)) {
	this->handlers.insert(std::make_pair(event, handler));
}

void Controller::fireEvent(std::string event) {
	void (*handler)(Controller & c) = this->handlers.at(event);
	handler(*this);
}

const std::string ControllerMain::eGameLose = "g-lose";
const std::string ControllerMain::eGameWin = "g-win";
const std::string ControllerMain::eReset = "g-reset";
const std::string ControllerMain::eExit = "g-exit";

ControllerMain::ControllerMain()
	:Controller()
{
	pPlayer = sf::Vector2<int>(0, 0);
	pEnemy = sf::Vector2<int>(0, 0);
	pFinish = sf::Vector2<int>(0, 0);
	solver = new Solver();
}

void ControllerMain::hookSetup() {
	pPlayer.x = 0, pPlayer.y = 0;
	pEnemy.x = 0, pEnemy.y = 0;
	pFinish.x = 0, pFinish.y = 0;
	memset(field, false, xFieldSize * yFieldSize);

	// Generate map
	for (int i = 0; i < obstAmount; i++)
	{
		int x = randomInt(xFieldSize);
		int y = randomInt(yFieldSize);

		for (int dx = 0; dx < obstSize; dx++) {
			for (int dy = 0; dy < obstSize; dy++) {
				if ((x + dx < xFieldSize) && (y + dy < yFieldSize)) {
					field[x + dx][y + dy] = true;
				}
			}
		}
	}

	do {
		// Get random start positions 
		// Make sure to not spawn player or enemy in obstacle
		pPlayer.x = randomInt(xFieldSize), pPlayer.y = randomInt(yFieldSize);
		pEnemy.x = randomInt(xFieldSize), pEnemy.y = randomInt(yFieldSize);
		pFinish.x = randomInt(xFieldSize), pFinish.y = randomInt(yFieldSize);
	} while (
		((pPlayer.x == pEnemy.x) & (pPlayer.y == pEnemy.y)) ||
		((pPlayer.x == pFinish.x) & (pPlayer.y == pFinish.y)) ||
		((pEnemy.x == pFinish.x) & (pEnemy.y == pFinish.y)) ||

		field[pPlayer.x][pPlayer.y] ||
		field[pEnemy.x][pEnemy.y] ||
		field[pFinish.x][pFinish.y]
		);


	/*for (int i = 0; i < xFieldSize; i++)
	{
		for (int j = 0; j < yFieldSize; j++)
		{
			std::cout << field[i][j];
		}
		std::cout << std::endl;
	}*/
}

void ControllerMain::hookArrowUp() {
	// Handle up movement, resolve collision
	if (pPlayer.y + 1 < yFieldSize && !field[pPlayer.x][pPlayer.y + 1]) {
		pPlayer.y += 1;
	}
}
void ControllerMain::hookArrowDown() {
	// Handle down movement, resolve collision
	if (pPlayer.y - 1 >= 0 && !field[pPlayer.x][pPlayer.y - 1]) {
		pPlayer.y -= 1;
	}
}
void ControllerMain::hookArrowRight() {
	// Handle left movement, resolve collision
	if (pPlayer.x + 1 < xFieldSize && !field[pPlayer.x + 1][pPlayer.y]) {
		pPlayer.x += 1;
	}
}
void ControllerMain::hookArrowLeft() {
	// Handle left movement, resolve collision
	if (pPlayer.x - 1 >= 0 && !field[pPlayer.x - 1][pPlayer.y]) {
		pPlayer.x -= 1;
	}
}

// Resolve main game logic
void ControllerMain::hookUpdateState() {
	// Game win
	if ((pPlayer.x == pFinish.x) & (pPlayer.y == pFinish.y)) {
		fireEvent(eGameWin);
		return;
	}

	if (updateCounter == updateTrigger) {

		tie(path, expanded) = solver->solve(pEnemy, pPlayer, field);
		std::cout << "x: " << pPlayer.x << ", y: " << pPlayer.y << ", exp: " << expanded.size() << ", pth: " << path.size() << std::endl;

		if (path.size()) {
			pEnemy.x = path[path.size() - 2].x;
			pEnemy.y = path[path.size() - 2].y;
		}

		// Game over
		if ((pPlayer.x == pEnemy.x) & (pPlayer.y == pEnemy.y)) {
			fireEvent(eGameLose);
			return;
		}

		updateCounter = 0;
	}
	else {
		updateCounter += 1;
	}
};

void ControllerMain::hookReturnKey() {}
void ControllerMain::hookEscKey() {}

void ControllerMain::draw(sf::RenderWindow* window) {
	sf::Color playerColor(0, 0, 255, 255);
	sf::Color playerShelfColor(0, 0, 255, 70);

	sf::Color enemyColor(255, 0, 0, 255);
	sf::Color enemyShelfColor(255, 0, 0, 70);

	sf::Color finishColor(255, 112, 67, 255);
	sf::Color finishShelfColor(216, 67, 21, 70);

	sf::Color cellColor(117, 117, 117, 255);
	sf::Color shelfColor(66, 66, 66, 255);

	sf::Color borderColor(100, 100, 100, 255);

	sf::Color expColor(255, 0, 0, 70);
	sf::Color pathColor(255, 0, 0, 120);

	for (int i = 0; i < xFieldSize; i++)
	{
		for (int j = 0; j < yFieldSize; j++)
		{
			if (field[i][j]) {
				sf::RectangleShape obst(sf::Vector2f(8.f, 8.f));
				obst.setPosition(sf::Vector2f(10.f * i + 1.f, 10.f * j + 1.f));
				obst.setFillColor(cellColor);
				window->draw(obst);

				if ((i > 0) & (j > 0))
					if (!field[i - 1][j - 1]) {
						sf::RectangleShape obst(sf::Vector2f(8.f, 8.f));
						obst.setPosition(sf::Vector2f(10.f * (i - 1) + 1.f, 10.f * (j - 1) + 1.f));
						obst.setFillColor(shelfColor);
						window->draw(obst);
					}
			}

		}
	}

	for (int i = 0; i < expanded.size(); i++) {
		sf::RectangleShape exp(sf::Vector2f(4.f, 4.f));
		exp.setPosition(sf::Vector2f(expanded[i].x * 10.f + 3.f, expanded[i].y * 10.f + 3.f));
		exp.setFillColor(expColor);
		window->draw(exp);
	}

	for (int i = 0; i < path.size(); i++) {
		sf::RectangleShape pth(sf::Vector2f(6.f, 6.f));
		pth.setPosition(sf::Vector2f(path[i].x * 10.f + 2.f, path[i].y * 10.f + 2.f));
		pth.setFillColor(pathColor);
		window->draw(pth);
	}


	sf::RectangleShape player(sf::Vector2f(10.f, 10.f));
	player.setPosition(sf::Vector2f(pPlayer.x * 10.f, pPlayer.y * 10.f));
	player.setFillColor(playerColor);
	window->draw(player);

	sf::RectangleShape playerShelf(sf::Vector2f(30.f, 30.f));
	playerShelf.setPosition(sf::Vector2f(pPlayer.x * 10.f - 10.f, pPlayer.y * 10.f - 10.f));
	playerShelf.setFillColor(playerShelfColor);
	window->draw(playerShelf);

	// Enemy
	sf::RectangleShape enemy(sf::Vector2f(10.f, 10.f));
	enemy.setPosition(sf::Vector2f(pEnemy.x * 10.f, pEnemy.y * 10.f));
	enemy.setFillColor(enemyColor);
	window->draw(enemy);

	sf::RectangleShape enemyShelf(sf::Vector2f(30.f, 30.f));
	enemyShelf.setPosition(sf::Vector2f(pEnemy.x * 10.f - 10.f, pEnemy.y * 10.f - 10.f));
	enemyShelf.setFillColor(enemyShelfColor);
	window->draw(enemyShelf);

	// Finish
	sf::RectangleShape finish(sf::Vector2f(10.f, 10.f));
	finish.setPosition(sf::Vector2f(pFinish.x * 10.f, pFinish.y * 10.f));
	finish.setFillColor(finishColor);
	window->draw(finish);

	sf::RectangleShape finishShelf(sf::Vector2f(30.f, 30.f));
	finishShelf.setPosition(sf::Vector2f(pFinish.x * 10.f - 10.f, pFinish.y * 10.f - 10.f));
	finishShelf.setFillColor(finishShelfColor);
	window->draw(finishShelf);
}