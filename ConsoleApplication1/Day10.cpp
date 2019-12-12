#include "Day10.h"

#define _USE_MATH_DEFINES

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <deque>
#include <cmath>
#include <math.h>
#include "helpers.h"

namespace day10 {

	class Asteroid {
	public:
		int x;
		int y;
		double angle = 0;
		double distance = 0;
		int visible = true;
		Asteroid(int x, int y) : x(x), y(y) {};
		void SetAngleFrom(Asteroid * center) {
			if (x == center->x && y == center->y) return;
			double relativeX = (double)this->x - center->x;
			double relativeY = (double)this->y - center->y;
			distance = std::abs(relativeX) + std::abs(relativeY);
			double atan = std::atan2(-relativeY, relativeX);
			if (relativeX >= 0 && relativeY <= 0) {
				angle = M_PI / 2 - atan;
			}
			else if (relativeX < 0 && relativeY <= 0 && atan > 0) {
				angle = (M_PI * 2) - atan + M_PI / 2;
			}
			else {
				angle = std::abs(atan) + M_PI / 2;
			}
		}
	};

	std::vector<Asteroid> ParseMap()
	{
		std::vector<Asteroid> output;
		std::fstream inf("Part10Input.txt");
		std::string line;
		int y = 0;
		while (std::getline(inf, line)) {
			if (line.empty()) break;
			int x = 0;
			for (char c : line) {
				if (c == '#') {
					Asteroid a(x, y);
					output.push_back(a);
				};
				x++;
			}
			y++;
		}
		return output;
	}


	int Day10::Part1(){
		auto amap = ParseMap();
		int maxX = 0;
		int maxY = 0;
		std::map<int, std::map<int, Asteroid *>> lookup;

		for (Asteroid& a : amap) {
			maxX = std::max(a.x, maxX);
			maxY = std::max(a.y, maxY);
			lookup[a.x][a.y] = &a;
		}

		for (const Asteroid& origin : amap) {
			//std::cout << origin.x << " , " << origin.y << std::endl;
		}

		int maxCount = 0;
		Asteroid* best = nullptr;

		for (Asteroid& origin : amap) {
			int visibleCount = amap.size() - 1;
			int x = origin.x;
			int y = origin.y;

			for (Asteroid& other : amap) {
				if (other.x == origin.x && other.y == origin.y) continue;
				if (!other.visible) continue;

				int dx = other.x - x;
				int dy = other.y - y;
				int gcd = abs(helpers::GCD(dx, dy));
				int rx = dx / gcd;
				int ry = dy / gcd;

				int mult = gcd + 1;
				while (x + rx * mult <= maxX && x + rx * mult >= 0 && y + ry * mult <= maxY && y + ry * mult >= 0) {
					Asteroid* maybeOther = lookup[x + rx * mult][y + ry * mult];
					if (maybeOther != nullptr && maybeOther->visible) {
						maybeOther->visible = false;
						visibleCount--;
					}
					mult++;
				}

			}

			//std::cout << x << " , " << y << " , " << visibleCount << std::endl;
			if (maxCount < visibleCount) {
				maxCount = visibleCount;
				best = &origin;
			}
			for (Asteroid& a: amap) {
				a.visible = true;
			}
		}
		std::cout << "Max count " << maxCount << " , " << best->x << " , " << best->y << std::endl;

		// Part2
		for (Asteroid& a : amap) {
			a.SetAngleFrom(best);
		}

		std::sort(amap.begin(), amap.end(), [](const Asteroid& a, const Asteroid& b) {
			if (a.angle == b.angle) return a.distance < b.distance;
			return a.angle < b.angle;
			});
		std::vector<std::deque<Asteroid>> eliminationQueue;
			
		double prevAngle = INFINITY;
		for (Asteroid a : amap) {
			if (a.distance == 0) { 
				continue;
			}
			std::cout << a.x << " , " << a.y << " , " << a.angle << std::endl;
			if (prevAngle == a.angle) {
				eliminationQueue[eliminationQueue.size() - 1].push_back(a);
			}
			else {
				std::deque<Asteroid> d = { a };
				eliminationQueue.push_back(d);
				prevAngle = a.angle;
			}
		}

		int currentIndex = 0;
		for (int i = 1; i <= 201 && i <= amap.size() - 1; i++) {
			while (eliminationQueue[currentIndex].empty()) currentIndex = (currentIndex + 1) % eliminationQueue.size();
			Asteroid a = eliminationQueue[currentIndex].front();
			eliminationQueue[currentIndex].pop_front();
			currentIndex = (currentIndex + 1) % eliminationQueue.size();
			std::cout << i << " ELIMINATED " << a.x << " , " << a.y << std::endl;
		}

		return 0;
	}





}