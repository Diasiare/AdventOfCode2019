#include "Day2.h"

#include <utility>

Computer::Computer(vector<int> initialState) : state_(initialState) {};

void Computer::RestoreState(int noun, int verb) {
	state_[1] = noun;
	state_[2] = verb;
}

int Computer::ValueAt(int pos) {
	return state_[state_[pos]];
}

void Computer::WriteTo(int pos, int value) {
	state_[state_[pos]] = value;
}

void Computer::ProcessOpcode1() {
	int v1 = ValueAt(currentPosition + 1);
	int v2 = ValueAt(currentPosition + 2);
	WriteTo(currentPosition + 3, v1 + v2);
}

void Computer::ProcessOpcode2() {
	int v1 = ValueAt(currentPosition + 1);
	int v2 = ValueAt(currentPosition + 2);
	WriteTo(currentPosition + 3, v1 * v2);
}

int Computer::Run() {

	while (currentPosition < state_.size()) {
		int opcode = state_[currentPosition];
		switch (state_[currentPosition]) {
		case 1:
			if (currentPosition + 3 >= state_.size()) return -1;
			ProcessOpcode1();
			break;
		case 2:
			if (currentPosition + 3 >= state_.size()) return -1;
			ProcessOpcode2();
			break;
		case 99:
			return state_[0];
		default:
			cout << "CRITICAL FAILURE " << currentPosition << endl;
			return -1;
		}
		currentPosition += 4;
	}
	return -1;
}



int Day2::Part1() {
	vector<int> input = {
		1,0,0,3,1,1,2,3,1,3,4,3,1,5,0,3,2,9,1,19,1,19,6,23,2,6,23,27,2,27,9,31,1,5,31,35,1,35,10,39,2,39,9,43,1,5,43,47,2,47,10,51,1,51,6,55,1,5,55,59,2,6,59,63,2,63,6,67,1,5,67,71,1,71,9,75,2,75,10,79,1,79,5,83,1,10,83,87,1,5,87,91,2,13,91,95,1,95,10,99,2,99,13,103,1,103,5,107,1,107,13,111,2,111,9,115,1,6,115,119,2,119,6,123,1,123,6,127,1,127,9,131,1,6,131,135,1,135,2,139,1,139,10,0,99,2,0,14,0
	};

	Computer computer(input);
	computer.RestoreState(12, 2);
	return computer.Run();
}

int Day2::Part2() {
	vector<int> input = {
		1,0,0,3,1,1,2,3,1,3,4,3,1,5,0,3,2,9,1,19,1,19,6,23,2,6,23,27,2,27,9,31,1,5,31,35,1,35,10,39,2,39,9,43,1,5,43,47,2,47,10,51,1,51,6,55,1,5,55,59,2,6,59,63,2,63,6,67,1,5,67,71,1,71,9,75,2,75,10,79,1,79,5,83,1,10,83,87,1,5,87,91,2,13,91,95,1,95,10,99,2,99,13,103,1,103,5,107,1,107,13,111,2,111,9,115,1,6,115,119,2,119,6,123,1,123,6,127,1,127,9,131,1,6,131,135,1,135,2,139,1,139,10,0,99,2,0,14,0
	};

	for (int noun = 0; noun < 100; ++noun) {
		for (int verb = 0; verb < 100; ++verb) {
			Computer computer(input);
			computer.RestoreState(noun, verb);
			int output = computer.Run();
			if (output == 19690720) return noun * 100 + verb;
		}
	}
	return -1;

}



