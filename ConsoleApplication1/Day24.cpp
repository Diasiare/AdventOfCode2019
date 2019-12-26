#include "Day24.h"

#include "helpers.h";
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <map>
#include <iostream>

namespace day24 {

	typedef std::vector<std::vector<bool>> World;

	World ParseInput() {
		std::string input_file = "Part24Input.txt";
		std::string input_string = helpers::ReadFile(input_file);
		World w;

		for (std::string line : helpers::SplitBy(input_string, "\n")) {
			std::vector<bool> row;
			for (char c : line) {
				row.push_back('#' == c);
			}
			w.push_back(row);
		}
		return w;
	}

	bool IsBug(int row, int column, World& w) {
		if (row < 0 || column < 0) return false;
		if (row >= w.size()) return false;
		if (column >= w[row].size()) return false;
		return w[row][column];
	}

	int CountNearby(int row, int column, World& w) {
		int count = 0;
		count += IsBug(row + 1, column, w);
		count += IsBug(row - 1, column, w);
		count += IsBug(row, column + 1, w);
		count += IsBug(row, column - 1, w);
		return count;
	}

	World Next(World & current) {
		World next;
		for (int i = 0; i < current.size(); i++) {
			std::vector<bool> next_row;
			for (int i2 = 0, i2End = current[i].size(); i2 < i2End; i2++) {
				bool isBug = current[i][i2];
				int nearby_count = CountNearby(i, i2, current);
				if (isBug && nearby_count != 1) isBug = false;
				else if (!isBug && nearby_count >= 1 && nearby_count <= 2) isBug = true;
				next_row.push_back(isBug);
			}
			next.push_back(next_row);
		}
		return next;
	}

	long long BiodiversityScore(World& w) {
		long long accum = 0;
		long long i = 1;
		for (auto row : w) {
			for (bool isBug : row) {
				if (isBug) accum += i;
				i *= 2;
			}
		}
		return accum;
	}


	long long Day24::Part1() {

		World current_world = ParseInput();
		std::set<World> previous_states;

		while (previous_states.count(current_world) == 0) {
			previous_states.insert(current_world);
			current_world = Next(current_world);
		}

		return BiodiversityScore(current_world);
	}

	typedef std::map<int, World> RWorld;

	struct Position {
		int row;
		int column;
		int layer;

		Position(int row, int column, int layer) : row(row), column(column), layer(layer) {}
	};

	bool IsBug(Position p , RWorld& rw) {
		if (p.row < 0 || p.column < 0) return false;
		World & w = rw[p.layer];
		if (p.row >= w.size()) return false;
		if (p.column >= w[p.row].size()) return false;
		return w[p.row][p.column];
	}


	std::vector<Position> NearbyPosition(Position& p) {
		std::vector<Position> nearby;
		if (p.row == 1 && p.column == 2) {
			nearby.push_back(Position(0, 0, p.layer + 1));
			nearby.push_back(Position(0, 1, p.layer + 1));
			nearby.push_back(Position(0, 2, p.layer + 1));
			nearby.push_back(Position(0, 3, p.layer + 1));
			nearby.push_back(Position(0, 4, p.layer + 1));

			nearby.push_back(Position(0, 2, p.layer));
			nearby.push_back(Position(1, 1, p.layer));
			nearby.push_back(Position(1, 3, p.layer));
			return nearby;
		} else if (p.row == 3 && p.column == 2) {
			nearby.push_back(Position(4, 0, p.layer + 1));
			nearby.push_back(Position(4, 1, p.layer + 1));
			nearby.push_back(Position(4, 2, p.layer + 1));
			nearby.push_back(Position(4, 3, p.layer + 1));
			nearby.push_back(Position(4, 4, p.layer + 1));

			nearby.push_back(Position(4, 2, p.layer));
			nearby.push_back(Position(3, 1, p.layer));
			nearby.push_back(Position(3, 3, p.layer));
			return nearby;
		} else if (p.row == 2 && p.column == 1) {
			nearby.push_back(Position(0, 0, p.layer + 1));
			nearby.push_back(Position(1, 0, p.layer + 1));
			nearby.push_back(Position(2, 0, p.layer + 1));
			nearby.push_back(Position(3, 0, p.layer + 1));
			nearby.push_back(Position(4, 0, p.layer + 1));

			nearby.push_back(Position(1, 1, p.layer));
			nearby.push_back(Position(2, 0, p.layer));
			nearby.push_back(Position(3, 1, p.layer));
			return nearby;
		}
		else if (p.row == 2 && p.column == 3) {
			nearby.push_back(Position(0, 4, p.layer + 1));
			nearby.push_back(Position(1, 4, p.layer + 1));
			nearby.push_back(Position(2, 4, p.layer + 1));
			nearby.push_back(Position(3, 4, p.layer + 1));
			nearby.push_back(Position(4, 4, p.layer + 1));

			nearby.push_back(Position(1, 3, p.layer));
			nearby.push_back(Position(2, 4, p.layer));
			nearby.push_back(Position(3, 3, p.layer));
			return nearby;
		}

		if (p.row == 0) {
			nearby.push_back(Position(1, 2, p.layer - 1));
			nearby.push_back(Position(1, p.column, p.layer));
		}
		else if (p.row == 4) {
			nearby.push_back(Position(3, 2, p.layer - 1));
			nearby.push_back(Position(3, p.column, p.layer));
		}
		else {
			nearby.push_back(Position(p.row - 1, p.column, p.layer));
			nearby.push_back(Position(p.row + 1, p.column, p.layer));
		}

		if (p.column == 0) {
			nearby.push_back(Position(2, 1, p.layer - 1));
			nearby.push_back(Position(p.row, 1, p.layer));
		}
		else if (p.column == 4) {
			nearby.push_back(Position(2, 3, p.layer - 1));
			nearby.push_back(Position(p.row, 3, p.layer));
		}
		else {
			nearby.push_back(Position(p.row, p.column + 1, p.layer));
			nearby.push_back(Position(p.row, p.column - 1, p.layer));
		}
		return nearby;
	}
	int CountNearby(Position p, RWorld& w) {
		auto nearby = NearbyPosition(p);
		int count = 0;
		for (auto& n : nearby) {
			count += IsBug(n, w);
		}
		return count;
	}

	void FindMinMaxWorld(RWorld& rw, int* min, int* max) {
		int lmin = 0;
		int lmax = 0;
		for (auto iw : rw) {
			if (iw.second.empty()) continue;
			lmin = std::min(lmin, iw.first);
			lmax = std::max(lmax, iw.first);
		}

		bool found = false;
		for (auto r : rw[lmin]) for (bool b : r) found = found || b;
		if (found) lmin--;

		found = false;
		for (auto r : rw[lmax]) for (bool b : r) found = found || b;
		if (found) lmax++;

		*min = lmin;
		*max = lmax;
	}

	constexpr int WORLD_SIZE = 5;

	RWorld Next(RWorld& current) {

		int minWorld = 0;
		int maxWorld = 0;
		FindMinMaxWorld(current, &minWorld, &maxWorld);

		RWorld next;
		for (int layer = minWorld; layer <= maxWorld; layer++) {
			World world;
			for (int row = 0; row < WORLD_SIZE; row++) {
				std::vector<bool> next_row;
				for (int col = 0; col < WORLD_SIZE; col++) {
					if (col == 2 && row == 2) {
						next_row.push_back(false);
						continue;
					}
					Position p(row, col, layer);
					bool isBug = IsBug(p, current);
					int nearby_count = CountNearby(p, current);
					if (isBug && nearby_count != 1) isBug = false;
					else if (!isBug && nearby_count >= 1 && nearby_count <= 2) isBug = true;
					next_row.push_back(isBug);
				}
				world.push_back(next_row);
			}
			next[layer] = world;
		}
		return next;
	}

	void Print(RWorld& rw) {
		std::string toPrint = "";
		for (auto l : rw) {
			if (l.second.empty()) continue;
			toPrint += "Layer: " + std::to_string(l.first) + " \n";
			for (auto row : l.second) {
				for (bool b : row) {
					toPrint += b ? "#" : ".";
				}
				toPrint += "\n";
			}
			toPrint += "\n";
		}
		std::cout << toPrint;
	}

	long long Day24::Part2() {
		World current_world_layer_0 = ParseInput();
		RWorld current;
		current[0] = current_world_layer_0;

		for (int i = 0; i < 200; i++) {
			Print(current);
			current = Next(current);
			std::cout << "Itteration: " << i + 1 << "-----------------\n\n";
		}
		Print(current);

		int accum = 0;
		for (auto l : current) for (auto r : l.second) for (auto b : r) {
			accum += b;
		}
		return accum;
	}
}