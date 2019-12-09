#pragma once

#include <unordered_map>
#include <string>
#include <unordered_set>

using namespace std;

namespace day3 {

	class Day3
	{
	public:
		static int Part1();
		static int Part2();
		static vector<pair<char, int>> ParseLine(const string&);
	};


	class Cabinet
	{
	private:
		unordered_map<int, unordered_map<int, unordered_map<int, int>>> repr;
		int lineCount;

	public:
		void AddLine(const vector<pair<char, int>>& line);
		int DistanceToClosestIntersection();
		int ClosestIntersection();
	};

}