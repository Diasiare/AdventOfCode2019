#include "Day6.h"

#include <fstream>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

using namespace std;

Planet::Planet(string name) : name(name)
{
}

void Planet::AddOrbit(shared_ptr<Planet> me, shared_ptr<Planet> planet)
{
	orbitedBy.push_back(planet);
	planet->parent = me;
}

int Planet::OrbitCount(int depth) {
	int count = depth;
	for (auto o : orbitedBy) {
		count += o->OrbitCount(depth + 1);
	}
	return count;
}

int Planet::FindPathLengthBetween(Planet& start, Planet& finish)
{
	unordered_set<string> visited;
	return start.DistanceTo(finish.name , visited);

}

int Planet::DistanceTo(string target, unordered_set<string> &visited) {
	if (target == name) return 0;
	visited.insert(name);

	if (name != "COM" && visited.count(parent->name) == 0) {
		int dist = parent->DistanceTo(target, visited);
		if (dist >= 0) return dist + 1;
	}

	for (auto child : orbitedBy) {
		if (visited.count(child->name) == 0) {
			int dist = child->DistanceTo(target, visited);
			if (dist >= 0) return dist + 1;
		}
	}
	return -1;
}

vector<pair<string, string>> Day6::ParseOrbitMap()
{
	vector<pair<string, string>> output;
	fstream inf("Part6Input.txt");
	string line;
	while (getline(inf, line)) {
		int pos = line.find(')');
		if (pos == string::npos) break;
		string p1 = line.substr(0, pos);
		string p2 = line.substr(pos + 1, line.size() - (pos + 1));
		output.push_back(make_pair(p1, p2));
	}
	return output;
}

int Day6::Part1()
{
	unordered_map<string, shared_ptr<Planet>> planets;
	auto orbits = ParseOrbitMap();
	for (auto orbit : orbits) {
		shared_ptr<Planet> center;
		if (planets.count(orbit.first) == 0) {
			center = make_shared<Planet>(orbit.first);
			planets[orbit.first] = center;
		}
		else {
			center = planets[orbit.first];
		}
		shared_ptr<Planet> outer;
		if (planets.count(orbit.second) == 0) {
			outer = make_shared<Planet>(orbit.second);
			planets[orbit.second] = outer;
		}
		else {
			outer = planets[orbit.second];
		}
		center->AddOrbit(center, outer);
	}
	return planets["COM"]->OrbitCount(0);
}

int Day6::Part2() {
	unordered_map<string, shared_ptr<Planet>> planets;
	auto orbits = ParseOrbitMap();
	for (auto orbit : orbits) {
		shared_ptr<Planet> center;
		if (planets.count(orbit.first) == 0) {
			center = make_shared<Planet>(orbit.first);
			planets[orbit.first] = center;
		}
		else {
			center = planets[orbit.first];
		}
		shared_ptr<Planet> outer;
		if (planets.count(orbit.second) == 0) {
			outer = make_shared<Planet>(orbit.second);
			planets[orbit.second] = outer;
		}
		else {
			outer = planets[orbit.second];
		}
		center->AddOrbit(center, outer);
	}
	return Planet::FindPathLengthBetween(*(planets["YOU"]->parent), *(planets["SAN"]->parent));

}
