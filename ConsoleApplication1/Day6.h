#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_set>

using namespace std;

class Day6
{
public:
	static vector<pair<string, string>> ParseOrbitMap();
	static int Part1();
	static int Part2();
};

class Planet {
public:
	string name;
	vector<shared_ptr<Planet>> orbitedBy;
	shared_ptr<Planet> parent;

	Planet(string);
	void AddOrbit(shared_ptr<Planet>, shared_ptr<Planet>);
	int OrbitCount(int depth);
	static int FindPathLengthBetween(Planet&, Planet&);
	int DistanceTo(string name, unordered_set<string> &visited);
};