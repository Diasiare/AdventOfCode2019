#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_set>

namespace day6 {

class Day6
{
public:
	static std::vector<std::pair<std::string, std::string>> ParseOrbitMap();
	static int Part1();
	static int Part2();
};

class Planet {
public:
	std::string name;
	std::vector<std::shared_ptr<Planet>> orbitedBy;
	std::shared_ptr<Planet> parent;

	Planet(std::string);
	void AddOrbit(std::shared_ptr<Planet>, std::shared_ptr<Planet>);
	int OrbitCount(int depth);
	static int FindPathLengthBetween(Planet&, Planet&);
	int DistanceTo(std::string name, std::unordered_set<std::string> &visited);
};

}