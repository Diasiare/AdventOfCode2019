#pragma once

#include <unordered_map>
#include <string>
#include <unordered_set>

namespace day3 {

	class Day3
	{
	public:
		static int Part1();
		static int Part2();
		static std::vector<std::pair<char, int>> ParseLine(const std::string&);
	};


	class Cabinet
	{
	private:
		std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, int>>> repr;
		int lineCount;

	public:
		void AddLine(const std::vector<std::pair<char, int>>& line);
		int DistanceToClosestIntersection();
		int ClosestIntersection();
	};

}