
#include "Computer.h"
#include <iostream>
#include <memory>
#include <array>

Computer::Computer(vector<int> initialState) : state_(initialState) {};
Computer::Computer(vector<int> initialState, shared_ptr<IODriver> ioDriver) : state_(initialState), ioDriver(ioDriver) {}

void Computer::RestoreState(int noun, int verb) {
	state_[1] = noun;
	state_[2] = verb;
}

int Computer::Read(int pos) {
	return Read(pos, 0);
}

int Computer::Read(int pos, int mode) {
	switch (mode) {
	case 0: return state_[state_[pos]];
	case 1: return state_[pos];
	default: 
		cout << "UNKNOWN MODE " << mode << endl;
		return -1;
	}
}


void Computer::Write(int pos, int value) {
	state_[state_[pos]] = value;
}

void Computer::ProcessOpcode1() {
	auto modes = GetModes<3>();
	int v1 = Read(currentPosition + 1, modes[0]);
	int v2 = Read(currentPosition + 2, modes[1]);
	Write(currentPosition + 3, v1 + v2);
}

void Computer::ProcessOpcode2() {
	auto modes = GetModes<3>();
	int v1 = Read(currentPosition + 1, modes[0]);
	int v2 = Read(currentPosition + 2, modes[1]);
	Write(currentPosition + 3, v1 * v2);
}

void Computer::ProcessOpcode3() {
	auto modes = GetModes<1>();
	Write(currentPosition + 1, ioDriver->Read());
}

void Computer::ProcessOpcode4() {
	auto modes = GetModes<1>();
	int v = Read(currentPosition + 1, modes[0]);
	ioDriver->Write(v);
}

void Computer::ProcessOpcode5() {
	auto modes = GetModes<2>();
	int v1 = Read(currentPosition + 1, modes[0]);
	int v2 = Read(currentPosition + 2, modes[1]);
	if (v1 != 0) currentPosition = v2 - 3;
}

void Computer::ProcessOpcode6() {
	auto modes = GetModes<2>();
	int v1 = Read(currentPosition + 1, modes[0]);
	int v2 = Read(currentPosition + 2, modes[1]);
	if (v1 == 0) currentPosition = v2 - 3;
}

void Computer::ProcessOpcode7() {
	auto modes = GetModes<3>();
	int v1 = Read(currentPosition + 1, modes[0]);
	int v2 = Read(currentPosition + 2, modes[1]);
	Write(currentPosition + 3, v1 < v2 ? 1 : 0);
}

void Computer::ProcessOpcode8() {
	auto modes = GetModes<3>();
	int v1 = Read(currentPosition + 1, modes[0]);
	int v2 = Read(currentPosition + 2, modes[1]);
	Write(currentPosition + 3, v1 == v2 ? 1 : 0);
}

bool Computer::HasSufficientLength(int length) {
	return currentPosition + length - 1 < state_.size();
}

int Computer::Run() {

	while (currentPosition < state_.size()) {
		int opcode = state_[currentPosition] % 100;
		int advance = 0;
		switch (opcode) {
		case 1:
			advance = 4;
			if (!HasSufficientLength(advance)) return -1;
			if (currentPosition + 3 >= state_.size()) return -1;
			ProcessOpcode1();
			break;
		case 2:
			advance = 4;
			if (!HasSufficientLength(advance)) return -1; 
			ProcessOpcode2();
			break;
		case 3:
			advance = 2;
			if (!HasSufficientLength(advance)) return -1;
			ProcessOpcode3();
			break;
		case 4:
			advance = 2;
			if (!HasSufficientLength(advance)) return -1;
			ProcessOpcode4();
			break;
		case 5:
			advance = 3;
			if (!HasSufficientLength(advance)) return -1;
			ProcessOpcode5();
			break;
		case 6:
			advance = 3;
			if (!HasSufficientLength(advance)) return -1;
			ProcessOpcode6();
			break;
		case 7:
			advance = 4;
			if (!HasSufficientLength(advance)) return -1;
			ProcessOpcode7();
			break;
		case 8:
			advance = 4;
			if (!HasSufficientLength(advance)) return -1;
			ProcessOpcode8();
			break;
		case 99:
			return state_[0];
		default:
			cout << "CRITICAL FAILURE " << currentPosition << endl;
			return -1;
		}
		currentPosition += advance;
	}
	return -1;
}