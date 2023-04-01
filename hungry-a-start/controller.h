#pragma once

#define GameTitle "Hungry A*"

#include <SFML/Graphics.hpp>
#include "solver_a_star.h"

/// <summary>
/// Hold and mutate state of game. Controller is directly binded to view.
/// </summary>
class Controller {
public:
	/// <summary>
	/// Register callback function to bind to given event.
	/// </summary>
	/// <param name="event">Event name to bind to.</param>
	/// <param name="handler">Function to call on event.</param>
	virtual void registerEvent(std::string event, void (*handler)(Controller &c));

	/// <summary>
	/// Called once after controller is created.
	/// </summary>
	virtual void hookSetup() = 0;

	/// <summary>
	/// Called every time after render.
	/// </summary>
	virtual void hookUpdateState() = 0;

	/// <summary>
	/// Called on arrow up.
	/// </summary>
	virtual void hookArrowUp() = 0;

	/// <summary>
	/// Called on arrow down.
	/// </summary>
	virtual void hookArrowDown() = 0;

	/// <summary>
	/// Called on left arrow.
	/// </summary>
	virtual void hookArrowLeft() = 0;

	/// <summary>
	/// Called on right arrow.
	/// </summary>
	virtual void hookArrowRight() = 0;

	/// <summary>
	/// Called on return key.
	/// </summary>
	virtual void hookReturnKey() = 0;

	/// <summary>
	/// Called on escape key.
	/// </summary>
	virtual void hookEscKey() = 0;

	/// <summary>
	/// Called to render view on passed window. Window is already prepared to draw.
	/// </summary>
	/// <param name="window">Target window.</param>
	virtual void draw(sf::RenderWindow* window) = 0;

	void render(sf::RenderWindow* window);


protected:
	/// <summary>
	/// Trigger even handler.
	/// </summary>
	/// <param name="event">Event identifier which was used to register handler.</param>
	virtual void fireEvent(std::string event);

	bool randomBool();
	int randomInt(int mx);

private:
	/// <summary>
	/// Collection of registered handlers.
	/// </summary>
	std::map<std::string, void (*)(Controller &c)> handlers;
};


class ControllerMain : public Controller {
public:
	ControllerMain();

	const static std::string eGameWin;
	const static std::string eGameLose;
	const static std::string eReset;
	const static std::string eExit;

	void hookSetup() override;

	void hookUpdateState() override;

	void hookArrowUp() override;
	void hookArrowDown() override;
	void hookArrowLeft() override;
	void hookArrowRight() override;

	void hookReturnKey() override;
	void hookEscKey() override;

	void draw(sf::RenderWindow* window) override;

	static const int xFieldSize = 100;
	static const int yFieldSize = 100;

	bool field[xFieldSize][yFieldSize] = { false };

	static const int obstSize = 7;
	static const int obstAmount = 40;


	// Assert two points has a possible connection on the map
	bool assertReachable(sf::Vector2f pFrom, sf::Vector2f pTo);
	bool getPathAStart(sf::Vector2f pFrom, sf::Vector2f pTo);
	Solver* solver;

	// Player
	sf::Vector2<int> pPlayer;
	// Enemy 
	sf::Vector2<int> pEnemy;
	// Count 0 -> update state -> x + 1 ... max = update trigger -> 0
	// When counter == trigger enemy make a turn
	int updateCounter = 0;  
	int updateTrigger = 2;
	// Finish
	sf::Vector2<int> pFinish;

	std::vector<sf::Vector2<int>> expanded;
	std::vector<sf::Vector2<int>> path;
};