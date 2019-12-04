#pragma once

#include <string>
class Day4
{
private:
	static bool MatchesCriteria(int);
	static int CountMatchesBetween(int, int);

	static bool MatchesCriteria2(int);
	static int CountMatchesBetween2(int, int);
public:
	static int Part1();
	static int Part2();
};

