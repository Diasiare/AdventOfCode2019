#include "Day15.h"

#include "Computer.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
#include <deque>
#include <set>
#include <functional>
#include <queue>
#include <algorithm>
#include <iostream>

namespace day15 {

	typedef std::deque<std::pair<int, int>> Path;

	class PathFinder : public computer::IODriver {
	public:
		int currentX = 0;
		int currentY = 0;
		int minX = INT_MAX;
		int minY = INT_MAX;
		int maxX = INT_MIN;
		int maxY = INT_MIN;
		int lastCommand = 0;
		std::unordered_map<int, std::unordered_map<int, int>> map;
		std::set<std::pair<int, int>> reachable;
		Path currentPath;
		bool stop = false;
		std::pair<int, int> oxygenAt;
		int distanceToOxygenSystem = 0;


		PathFinder() {
			PositionExplored(std::make_pair(currentX, currentY), 2);
			auto dest = NextDestination();
			currentPath = FindPathBetween(std::make_pair(currentX, currentY), dest);
			currentPath.pop_front();
		}

		NUM_TYPE Read() override {
			auto to = currentPath.front();
			currentPath.pop_front();
			lastCommand = PointToDirection(to);
			return lastCommand;
		}

		void Write(NUM_TYPE code) override {
			int squareType = code + 1;
			auto point = DirectionToPoint(lastCommand);
			PositionExplored(point, squareType);
			
			if (squareType == 3) {
				oxygenAt = point;
				currentX = point.first;
				currentY = point.second;
			}

			if (reachable.size() == 0) {
				stop = true;
				distanceToOxygenSystem = FindPathBetween(std::make_pair(0, 0), oxygenAt).size() -1 ;
			}
			if (squareType == 2) {
				currentX = point.first;
				currentY = point.second;
			}
			if (currentPath.empty()) {
				auto dest = NextDestination();
				currentPath = FindPathBetween(std::make_pair(currentX, currentY),  dest);
				currentPath.pop_front();
				Print();
			}
		}

		void Print() {
			std::string tp = "";
			for (int y = minY - 1; y <= maxY + 1; y++) {
				for (int x = minX - 1; x <= maxX + 1; x++)
				{
					int code = map[y][x];
					std::string c = " ";
					switch (code) {
					case 0: c = " "; break;
					case 1: c = "#"; break;
					case 2: c = " "; break;
					case 3: c = "O"; break;
					}
					if (x == currentX && y == currentY) {
						c = "R";
					}
					if (reachable.count(std::make_pair(x, y)) > 0) {
						c = "?";
					}
					auto g = currentPath.back();
					if (g.first == x && g.second == y) {
						c = "G";
					}
					tp += c;
				}
				tp += "\n";
			}
			std::cout << tp << std::endl << std::endl;
		}

		void PositionExplored(const std::pair<int, int> & position, int type) {
			map[position.second][position.first] = type;
			reachable.erase(position);
			minX = std::min(minX, position.first);
			minY = std::min(minY, position.second);
			maxX = std::max(maxX, position.first);
			maxY = std::max(maxY, position.second);

			// Open or oxygen
			if (type >= 2) {
				AddToReachable(std::make_pair(position.first + 1, position.second));
				AddToReachable(std::make_pair(position.first - 1, position.second));
				AddToReachable(std::make_pair(position.first, position.second + 1));
				AddToReachable(std::make_pair(position.first, position.second - 1));
			}
		}

		void AddToReachable(std::pair<int, int> position) {
			if (map[position.second][position.first] == 0) {
				reachable.emplace(position);
			}
		}

		std::pair<int, int> DirectionToPoint(int direction) {
			switch (direction) {
			case 1: return std::make_pair(currentX, currentY + 1);
			case 2: return std::make_pair(currentX, currentY - 1);
			case 3: return std::make_pair(currentX - 1, currentY);
			case 4: return std::make_pair(currentX + 1, currentY);
			default: std::exit(10);
			}
		}

		int PointToDirection(std::pair<int, int> point) {
			if (point.first > currentX) return 4;
			if (point.first < currentX) return 3;
			if (point.second > currentY) return 1;
			if (point.second < currentY) return 2;
			exit(11);
		}

		bool Stop() override {
			return stop;
		}

		std::pair<int, int> NextDestination() {
			std::pair<int, int> out;
			int pathLength = INT_MAX;
			for (auto& p : reachable) {
				int dist = Distance(std::make_pair(currentX, currentY), p);
				if (pathLength > dist) {
					pathLength = dist;
					out = p;
				};
			}
			return out;
		}

		static int Distance(const std::pair<int, int> & a, const std::pair<int, int> & b) {
			return std::abs(a.first - b.first) + std::abs(a.second - b.second);
		}

		static int Score(const Path & path, const std::pair<int, int> & destination) {
			return path.size() - 1 + Distance(path.back(), destination);
		}

		void MaybeEmplace(std::priority_queue<Path, std::vector<Path>, std::function<bool(const Path&, const Path&)>> & paths, 
			const std::set<std::pair<int, int>> & seen,
			Path path, std::pair<int, int> end) {
			if (seen.count(end) > 0) return;
			if (map[end.second][end.first] == 1) return;
			if (map[end.second][end.first] == 0 && reachable.count(end) == 0) return;
			path.push_back(end);
			paths.emplace(path);
		}

		Path FindPathBetween(const std::pair<int, int>& from, const std::pair<int, int>& to) {
			auto comp = [to](const Path& a, const Path& b) {
				int sa = Score(a, to);
				int sb = Score(b, to);
				if (sa != sb) return sa > sb;
				if (a.size() != b.size()) a.size() > b.size();
				return false;
			};

			std::priority_queue<Path, std::vector<Path>, std::function<bool(const Path &, const Path &)>> paths(comp);
			std::set<std::pair<int, int>> seen;

			Path startPath = { from };
			paths.emplace(startPath);
			while (!paths.empty()) {
				auto c = paths.top();
				paths.pop();

				if (c.back() == to) return c;
				const auto & end = c.back();
				seen.emplace(end);
				MaybeEmplace(paths, seen, c, std::make_pair(end.first + 1, end.second));
				MaybeEmplace(paths, seen, c, std::make_pair(end.first - 1, end.second));
				MaybeEmplace(paths, seen, c, std::make_pair(end.first, end.second + 1));
				MaybeEmplace(paths, seen, c, std::make_pair(end.first, end.second - 1));
			};

			std::exit(12);
		}

		void ShouldSpreadTo(const std::set<std::pair<int, int>>& left,
			std::set<std::pair<int, int>>& next, std::pair<int, int> point) {
			if (map[point.second][point.first] != 2) return;
			if (left.count(point) == 0) return;
			next.emplace(point);
		}

		int DoOxygenSpread() {
			std::set<std::pair<int, int>> leftToSpreadTo;
			std::set<std::pair<int, int>> nextWave = { oxygenAt };
			int minutes = -1;
			for (const auto& yM : map) {
				for (const auto& x : yM.second) {
					if (x.second == 2) {
						leftToSpreadTo.insert(std::make_pair(x.first, yM.first));
					}
				}
			}

			while (!leftToSpreadTo.empty()) {
				std::set<std::pair<int, int>> nnextWave;
				for (const auto& p : nextWave) {
					leftToSpreadTo.erase(p);
					map[p.second][p.first] = 3;
					ShouldSpreadTo(leftToSpreadTo, nnextWave, std::make_pair(p.first + 1, p.second));
					ShouldSpreadTo(leftToSpreadTo, nnextWave, std::make_pair(p.first - 1, p.second));
					ShouldSpreadTo(leftToSpreadTo, nnextWave, std::make_pair(p.first, p.second + 1));
					ShouldSpreadTo(leftToSpreadTo, nnextWave, std::make_pair(p.first, p.second - 1));
				}
				nextWave = nnextWave;
				minutes++;
				Print();
			}

			return minutes;
		}

	};

	int Day15::Part1() {
		std::vector<NUM_TYPE> code = {
			3,1033,1008,1033,1,1032,1005,1032,31,1008,1033,2,1032,1005,1032,58,1008,1033,3,1032,1005,1032,81,1008,1033,4,1032,1005,1032,104,99,101,0,1034,1039,101,0,1036,1041,1001,1035,-1,1040,1008,1038,0,1043,102,-1,1043,1032,1,1037,1032,1042,1105,1,124,101,0,1034,1039,102,1,1036,1041,1001,1035,1,1040,1008,1038,0,1043,1,1037,1038,1042,1106,0,124,1001,1034,-1,1039,1008,1036,0,1041,102,1,1035,1040,1001,1038,0,1043,1001,1037,0,1042,1106,0,124,1001,1034,1,1039,1008,1036,0,1041,1001,1035,0,1040,1001,1038,0,1043,1002,1037,1,1042,1006,1039,217,1006,1040,217,1008,1039,40,1032,1005,1032,217,1008,1040,40,1032,1005,1032,217,1008,1039,7,1032,1006,1032,165,1008,1040,5,1032,1006,1032,165,1102,1,2,1044,1105,1,224,2,1041,1043,1032,1006,1032,179,1101,0,1,1044,1105,1,224,1,1041,1043,1032,1006,1032,217,1,1042,1043,1032,1001,1032,-1,1032,1002,1032,39,1032,1,1032,1039,1032,101,-1,1032,1032,101,252,1032,211,1007,0,27,1044,1106,0,224,1102,1,0,1044,1106,0,224,1006,1044,247,101,0,1039,1034,101,0,1040,1035,102,1,1041,1036,1001,1043,0,1038,102,1,1042,1037,4,1044,1106,0,0,13,3,18,86,2,10,5,16,95,16,54,4,23,63,70,10,21,20,26,99,85,9,96,3,83,5,9,91,14,1,4,78,11,15,53,10,35,13,7,17,30,90,23,65,65,67,16,4,65,39,11,57,13,36,22,95,53,63,22,47,12,47,2,12,3,71,92,17,55,16,51,79,6,3,92,15,17,15,18,63,8,12,3,49,6,69,32,1,25,83,17,12,1,76,23,95,17,13,92,13,56,16,69,94,11,20,31,83,30,21,88,22,61,45,6,70,12,3,30,23,86,6,93,4,24,9,73,72,7,72,83,9,30,6,24,86,99,11,11,96,16,68,10,35,19,23,6,79,51,8,3,8,75,2,32,26,73,23,80,30,86,25,64,46,24,81,20,18,85,7,94,28,37,93,18,12,77,99,14,22,19,50,2,18,45,63,8,2,89,79,79,7,33,77,18,20,22,12,58,61,20,4,58,20,51,79,14,32,19,87,21,19,76,8,81,7,13,72,75,22,28,22,14,92,30,18,90,10,6,97,25,34,9,20,26,52,45,6,4,97,4,46,26,86,61,20,25,28,26,22,54,69,16,51,3,58,5,23,75,92,18,98,12,11,55,38,22,87,14,20,17,52,73,9,91,30,14,26,12,56,81,54,9,72,18,12,47,93,22,54,21,59,73,7,78,12,87,26,5,39,45,4,55,16,21,86,62,20,98,61,14,20,70,14,25,92,32,44,2,3,15,32,23,23,97,76,78,15,23,95,21,11,69,34,12,89,3,95,24,15,59,38,39,72,14,15,55,48,18,2,43,26,13,58,68,11,22,89,33,79,22,43,40,14,26,5,50,11,28,9,36,33,2,22,43,21,90,15,92,14,14,49,9,80,14,85,99,70,8,16,14,15,70,1,39,32,45,5,57,12,12,4,99,75,28,14,2,28,71,5,69,61,4,28,98,97,87,10,80,2,65,93,6,21,81,7,95,22,35,18,38,23,11,53,14,5,2,84,3,70,33,19,8,52,10,99,14,58,36,1,3,30,53,4,7,47,10,93,2,32,17,40,68,43,20,41,4,16,21,29,23,82,2,18,37,37,15,19,26,41,28,9,95,17,17,52,25,13,49,28,47,22,5,52,14,21,72,83,7,17,86,20,3,18,58,14,19,25,56,65,65,26,53,8,20,75,31,21,40,17,6,33,20,95,47,24,75,26,17,96,24,48,65,97,4,52,20,78,47,14,23,77,32,8,18,98,43,7,61,25,84,40,6,36,24,87,24,71,77,13,20,49,16,60,35,9,64,48,21,2,74,25,1,2,57,11,58,7,45,35,26,13,74,92,2,9,82,9,20,23,15,33,94,7,10,48,78,16,24,94,33,11,21,5,89,47,15,52,12,51,51,81,9,18,39,14,2,97,79,33,23,12,99,3,16,11,79,83,45,18,23,78,86,69,10,25,98,62,62,18,7,44,47,1,3,92,8,22,81,9,3,29,8,81,21,13,95,6,5,99,5,29,16,3,53,72,26,14,44,97,7,43,12,42,65,17,8,12,88,55,18,20,34,13,39,10,72,58,15,11,69,17,94,20,22,52,28,13,30,65,8,2,63,18,4,36,17,8,71,16,71,15,64,14,31,51,75,1,12,92,14,35,23,40,45,1,5,87,28,18,83,43,9,90,2,3,50,18,61,68,5,89,16,44,7,34,82,74,15,83,15,70,13,80,20,43,8,35,14,58,50,75,20,50,9,68,46,52,2,73,11,60,32,61,25,40,9,31,21,73,0,0,21,21,1,10,1,0,0,0,0,0,0
		};

		std::shared_ptr<PathFinder> driver = std::make_shared<PathFinder>();

		computer::Computer c(code, driver);
		c.Run();

		return driver->distanceToOxygenSystem;
		
	}

	int Day15::Part2() {
		std::vector<NUM_TYPE> code = {
	3,1033,1008,1033,1,1032,1005,1032,31,1008,1033,2,1032,1005,1032,58,1008,1033,3,1032,1005,1032,81,1008,1033,4,1032,1005,1032,104,99,101,0,1034,1039,101,0,1036,1041,1001,1035,-1,1040,1008,1038,0,1043,102,-1,1043,1032,1,1037,1032,1042,1105,1,124,101,0,1034,1039,102,1,1036,1041,1001,1035,1,1040,1008,1038,0,1043,1,1037,1038,1042,1106,0,124,1001,1034,-1,1039,1008,1036,0,1041,102,1,1035,1040,1001,1038,0,1043,1001,1037,0,1042,1106,0,124,1001,1034,1,1039,1008,1036,0,1041,1001,1035,0,1040,1001,1038,0,1043,1002,1037,1,1042,1006,1039,217,1006,1040,217,1008,1039,40,1032,1005,1032,217,1008,1040,40,1032,1005,1032,217,1008,1039,7,1032,1006,1032,165,1008,1040,5,1032,1006,1032,165,1102,1,2,1044,1105,1,224,2,1041,1043,1032,1006,1032,179,1101,0,1,1044,1105,1,224,1,1041,1043,1032,1006,1032,217,1,1042,1043,1032,1001,1032,-1,1032,1002,1032,39,1032,1,1032,1039,1032,101,-1,1032,1032,101,252,1032,211,1007,0,27,1044,1106,0,224,1102,1,0,1044,1106,0,224,1006,1044,247,101,0,1039,1034,101,0,1040,1035,102,1,1041,1036,1001,1043,0,1038,102,1,1042,1037,4,1044,1106,0,0,13,3,18,86,2,10,5,16,95,16,54,4,23,63,70,10,21,20,26,99,85,9,96,3,83,5,9,91,14,1,4,78,11,15,53,10,35,13,7,17,30,90,23,65,65,67,16,4,65,39,11,57,13,36,22,95,53,63,22,47,12,47,2,12,3,71,92,17,55,16,51,79,6,3,92,15,17,15,18,63,8,12,3,49,6,69,32,1,25,83,17,12,1,76,23,95,17,13,92,13,56,16,69,94,11,20,31,83,30,21,88,22,61,45,6,70,12,3,30,23,86,6,93,4,24,9,73,72,7,72,83,9,30,6,24,86,99,11,11,96,16,68,10,35,19,23,6,79,51,8,3,8,75,2,32,26,73,23,80,30,86,25,64,46,24,81,20,18,85,7,94,28,37,93,18,12,77,99,14,22,19,50,2,18,45,63,8,2,89,79,79,7,33,77,18,20,22,12,58,61,20,4,58,20,51,79,14,32,19,87,21,19,76,8,81,7,13,72,75,22,28,22,14,92,30,18,90,10,6,97,25,34,9,20,26,52,45,6,4,97,4,46,26,86,61,20,25,28,26,22,54,69,16,51,3,58,5,23,75,92,18,98,12,11,55,38,22,87,14,20,17,52,73,9,91,30,14,26,12,56,81,54,9,72,18,12,47,93,22,54,21,59,73,7,78,12,87,26,5,39,45,4,55,16,21,86,62,20,98,61,14,20,70,14,25,92,32,44,2,3,15,32,23,23,97,76,78,15,23,95,21,11,69,34,12,89,3,95,24,15,59,38,39,72,14,15,55,48,18,2,43,26,13,58,68,11,22,89,33,79,22,43,40,14,26,5,50,11,28,9,36,33,2,22,43,21,90,15,92,14,14,49,9,80,14,85,99,70,8,16,14,15,70,1,39,32,45,5,57,12,12,4,99,75,28,14,2,28,71,5,69,61,4,28,98,97,87,10,80,2,65,93,6,21,81,7,95,22,35,18,38,23,11,53,14,5,2,84,3,70,33,19,8,52,10,99,14,58,36,1,3,30,53,4,7,47,10,93,2,32,17,40,68,43,20,41,4,16,21,29,23,82,2,18,37,37,15,19,26,41,28,9,95,17,17,52,25,13,49,28,47,22,5,52,14,21,72,83,7,17,86,20,3,18,58,14,19,25,56,65,65,26,53,8,20,75,31,21,40,17,6,33,20,95,47,24,75,26,17,96,24,48,65,97,4,52,20,78,47,14,23,77,32,8,18,98,43,7,61,25,84,40,6,36,24,87,24,71,77,13,20,49,16,60,35,9,64,48,21,2,74,25,1,2,57,11,58,7,45,35,26,13,74,92,2,9,82,9,20,23,15,33,94,7,10,48,78,16,24,94,33,11,21,5,89,47,15,52,12,51,51,81,9,18,39,14,2,97,79,33,23,12,99,3,16,11,79,83,45,18,23,78,86,69,10,25,98,62,62,18,7,44,47,1,3,92,8,22,81,9,3,29,8,81,21,13,95,6,5,99,5,29,16,3,53,72,26,14,44,97,7,43,12,42,65,17,8,12,88,55,18,20,34,13,39,10,72,58,15,11,69,17,94,20,22,52,28,13,30,65,8,2,63,18,4,36,17,8,71,16,71,15,64,14,31,51,75,1,12,92,14,35,23,40,45,1,5,87,28,18,83,43,9,90,2,3,50,18,61,68,5,89,16,44,7,34,82,74,15,83,15,70,13,80,20,43,8,35,14,58,50,75,20,50,9,68,46,52,2,73,11,60,32,61,25,40,9,31,21,73,0,0,21,21,1,10,1,0,0,0,0,0,0
		};

		std::shared_ptr<PathFinder> driver = std::make_shared<PathFinder>();

		computer::Computer c(code, driver);
		c.Run();

		return driver->DoOxygenSpread();
	}
}