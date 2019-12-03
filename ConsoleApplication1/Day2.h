#pragma once
#include <vector>
#include <iostream>

using namespace std;

class Day2
{
public:
	static int Part1();

	static int Part2();

};

class Computer {
private:
	vector<int> state_;
	int currentPosition = 0;
	int ValueAt(int);
	void WriteTo(int, int);
	void ProcessOpcode1();
	void ProcessOpcode2();

public:
	Computer(vector<int> initialState);
	void RestoreState(int, int);
	int Run();
}; 