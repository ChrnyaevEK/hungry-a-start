#include "solver_a_star.h"
#include <algorithm>

N::N(sf::Vector2<int> point, double fScore) {
	this->point = point;
	this->fScore = fScore;
}

bool operator== (const N& nA, const N& nB) {
	return nA.fScore == nB.fScore;
}

bool operator!= (const N& nA, const N& nB) {
	return nA.fScore != nB.fScore;
}

bool operator>= (const N& nA, const N& nB) {
	return nA.fScore >= nB.fScore;
}

bool operator> (const N& nA, const N& nB) {
	return nA.fScore > nB.fScore;
}

bool operator<= (const N& nA, const N& nB) {
	return nA.fScore <= nB.fScore;
}

bool operator< (const N& nA, const N& nB) {
	return nA.fScore < nB.fScore;
}

Solver::Solver() {

}

double Solver::metrics(sf::Vector2<int> pFrom, sf::Vector2<int> pTo) {
	return std::sqrt(std::pow(pTo.x - pFrom.x, 2) + std::pow(pTo.y - pFrom.y, 2));
}

int Solver::reduceDimension(sf::Vector2<int> p) {
	// If map size is known we may map 2D to 1D
	// 0;0 0;1 ---> 0 (0;0), 1(0;1), 2(1;0), 3(1;1)
	// 1;0 1;1
	return 100 * p.y + p.x;
}

sf::Vector2<int> Solver::expandDimension(int reducedDimension) {
	// Complementary operation to reduce dimension: 1D -> 2D
	int x = reducedDimension % 100;
	return sf::Vector2<int>(x, (reducedDimension - x) / 100);
}

std::tuple<
	std::vector<sf::Vector2<int>>,  // Path
	std::vector<sf::Vector2<int>>   // Expanded nodes
> Solver::solve(sf::Vector2<int> pFrom, sf::Vector2<int> pTo, bool subjectTo[100][100]) {
	std::priority_queue <N, std::vector<N>, std::greater<N> > toOpen;
	std::vector<int> toOpenUnsortedCopy;

	std::map<int, int> reverseTacker;
	std::map<int, double> gScore;
	std::map<int, double> fScore;

	std::vector<sf::Vector2<int>> path;
	std::vector<int> expanded;

	N start(pFrom, 0);
	toOpen.push(start);
	toOpenUnsortedCopy.push_back(reduceDimension(start.point));

	gScore[reduceDimension(start.point)] = 0;
	fScore[reduceDimension(start.point)] = metrics(start.point, pTo);

	while (!toOpen.empty()) {
		N current = toOpen.top();
		toOpen.pop();

		int currentReducedPointer = reduceDimension(current.point);

		if ((current.point.x == pTo.x) & (current.point.y == pTo.y)) {
			// Reconstruct path
			path.push_back(expandDimension(currentReducedPointer));
			while (reverseTacker[currentReducedPointer] != NULL) {
				currentReducedPointer = reverseTacker[currentReducedPointer];
				path.push_back(expandDimension(currentReducedPointer));
			}

			std::vector<sf::Vector2<int>> exp;
			for (int ii = 0; ii < expanded.size(); ii++) {
				exp.push_back(expandDimension(expanded[ii]));
			}

			return std::make_tuple(path, exp);
		}

		std::vector<sf::Vector2<int>> inc = expand(current.point, subjectTo);
		for (unsigned int i = 0; i < inc.size(); i++) {
			double tentativeGScore = gScore[currentReducedPointer] + metrics(current.point, inc[i]);
			int reducedPointer = reduceDimension(inc[i]);

			if (
				gScore.find(reducedPointer) == gScore.end() ||
				tentativeGScore < gScore[reducedPointer]
				) {
				// Inc node has never been reached before and should has inf. g score 
				// or it's g score is heigher then current path to inc node

				N incN(inc[i], tentativeGScore + metrics(inc[i], pTo));

				reverseTacker[reducedPointer] = currentReducedPointer;
				gScore[reducedPointer] = tentativeGScore;
				fScore[reducedPointer] = incN.fScore;

				if (std::find(toOpenUnsortedCopy.begin(), toOpenUnsortedCopy.end(), reducedPointer) == toOpenUnsortedCopy.end()) {
					toOpen.push(incN);
					toOpenUnsortedCopy.push_back(reducedPointer);
				}
			}
		}

		std::vector<int>::iterator expPos = std::find(expanded.begin(), expanded.end(), currentReducedPointer);
		if (expPos == expanded.end())
			expanded.push_back(currentReducedPointer);

		std::vector<int>::iterator opPos = std::find(toOpenUnsortedCopy.begin(), toOpenUnsortedCopy.end(), currentReducedPointer);
		if (opPos != toOpenUnsortedCopy.end())
			toOpenUnsortedCopy.erase(opPos);
	}

	std::vector<sf::Vector2<int>> exp;
	for (int ii = 0; ii < expanded.size(); ii++) {
		exp.push_back(expandDimension(expanded[ii]));
	}

	return std::make_tuple(path, exp);
}

/// <summary>
/// Return vector of incident nodes that are not occupied
/// </summary>
/// <param name="p">Target point to expand</param>
/// <param name="subjectTo">Boolean map to expand on</param>
/// <returns>Vector of incident nodes</returns>
std::vector<sf::Vector2<int>> Solver::expand(sf::Vector2<int> p, bool subjectTo[100][100]) {
	std::vector<sf::Vector2<int>> inc;

	// Each point has 8 possible incident nodes (points)
	for (int i = 0; i < 8; i++) {
		// Make a move in predefined direction
		int x = p.x + this->expansionDifferentials[i][0];
		int y = p.y + this->expansionDifferentials[i][1];

		// Position is not occupied and is not off map
		if ((x >= 0) & (x < 100) & (y >= 0) & (y < 100)) {
			if (!subjectTo[x][y]) {
				inc.push_back(sf::Vector2<int>(x, y));
			}
		}
	}

	return inc;
}

