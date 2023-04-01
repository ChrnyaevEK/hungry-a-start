#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>

class N {
public:
	N(sf::Vector2<int> point, double fScore);

	sf::Vector2<int> point;
	double fScore;

	friend bool operator== (const N& nA, const N& nB);
	friend bool operator!= (const N& nA, const N& nB);

	friend bool operator>= (const N& nA, const N& nB);
	friend bool operator> (const N& nA, const N& nB);

	friend bool operator<= (const N& nA, const N& nB);
	friend bool operator< (const N& nA, const N& nB);
};


class Solver {
public:
	Solver();
	std::tuple<
		std::vector<sf::Vector2<int>>,  // Path
		std::vector<sf::Vector2<int>>   // Expanded nodes
	> solve(sf::Vector2<int> pFrom, sf::Vector2<int> pTo, bool subjectTo[100][100]);
private:
	double metrics(sf::Vector2<int> pFrom, sf::Vector2<int> pTo);


	// Expand point on map to vector of incident nodes
	std::vector<sf::Vector2<int>> expand(sf::Vector2<int> p, bool subjectTo[100][100]);
	// new point = [point.x + diff[0], point.y + diff[1]];
	const int expansionDifferentials[8][8] = {
		{-1, 1},  // upper left -> ...
		{0, 1},
		{1, 1},
		{-1, 0},
		{1, 0},
		{-1, -1},
		{0, -1},
		{1, -1}, // ... -> lower right
	};

	int reduceDimension(sf::Vector2<int> p);
	sf::Vector2<int> expandDimension(int reducedDimension);
};


