#include "Day4.h"

#include <string>

using namespace std;
using namespace day4;

bool Day4::MatchesCriteria(int input)
{
	bool hasSequence = false;
	bool isIncreasing = true;
	string asString = std::to_string(input);

	for (int i = 0; i < asString.size() - 1; i++) {
		isIncreasing = isIncreasing && asString[i] <= asString[i + 1];
		hasSequence = hasSequence || asString[i] == asString[i + 1];
	}

	return hasSequence && isIncreasing;
}

int Day4::CountMatchesBetween(int start, int end)
{
	int count = 0;
	for (; start <= end; start++) {
		if (MatchesCriteria(start)) count++;
	}
	return count;
}

int Day4::Part1()
{

	return CountMatchesBetween(382345, 843167);
}

bool Day4::MatchesCriteria2(int input)
{
	bool hasSequence = false;
	bool isIncreasing = true;
	string asString = std::to_string(input);

	for (int i = 0; i < asString.size() - 1; i++) {
		char c = asString[i];
		isIncreasing = isIncreasing && c <= asString[i + 1];
		hasSequence = hasSequence ||
			(c == asString[i + 1] &&
			(i < 1 || asString[i - 1] != c) &&
				(i + 2 >= asString.size() || asString[i + 2] != c));
	}

	return hasSequence && isIncreasing;
}

int Day4::CountMatchesBetween2(int start, int end)
{
	int count = 0;
	for (; start <= end; start++) {
		if (MatchesCriteria2(start)) count++;
	}
	return count;
}

int Day4::Part2()
{

	return CountMatchesBetween2(382345, 843167);
}

