
#include "Computer.h"
#include <iostream>
#include <memory>
#include <array>

using namespace computer;
using namespace std;

StandardIODriver::StandardIODriver(NUM_TYPE v) : input(v) {};

void StandardIODriver::Write(NUM_TYPE value) {
	outputs.push_back(value);
};

NUM_TYPE StandardIODriver::Read() {
	return input;
};

constexpr bool debug_disabled = true;
#define dlog \
    if (debug_disabled) {} \
    else std::cout

Computer::Computer(vector<NUM_TYPE> & initialState) : Computer(initialState, nullptr) {};
Computer::Computer(vector<NUM_TYPE> & initialState, shared_ptr<IODriver> ioDriver) : ioDriver(ioDriver) {
	for (int i = 0; i < initialState.size(); i++) {
		state_[i] = initialState[i];
	}
}

void Computer::RestoreState(NUM_TYPE noun, NUM_TYPE verb) {
	state_[1] = noun;
	state_[2] = verb;
}

NUM_TYPE Computer::Read(int pos) {
	return Read(pos, 0);
}

NUM_TYPE Computer::Read(int pos, int mode) {
	NUM_TYPE out;
	switch (mode) {
	case 0: out = state_[state_[pos]];
		break;
	case 1: out = state_[pos];
		break;
	case 2: out = state_[relativeBase + state_[pos]];
		break;
	default:
		cout << "UNKNOWN MODE IN READ " << mode << endl;
		return -1;
	}
	dlog << "R(" << pos << "," << mode << ")=" << out << ",";
	return out;
}


void Computer::Write(int pos, NUM_TYPE value, int mode) {
	dlog << "W(" << pos << "," << mode << ")=" << value << ",";
	switch (mode) {
	case 0: state_[state_[pos]] = value;
		break;
	case 2: state_[relativeBase + state_[pos]] = value;
		break;
	default:
		cout << "UNKNOWN MODE IN WRITE " << mode << endl;
	}
}

void Computer::ProcessOpcode1() {
	auto modes = GetModes<3>();
	NUM_TYPE v1 = Read(currentPosition + 1, modes[0]);
	NUM_TYPE v2 = Read(currentPosition + 2, modes[1]);
	Write(currentPosition + 3, v1 + v2, modes[2]);
}

void Computer::ProcessOpcode2() {
	auto modes = GetModes<3>();
	NUM_TYPE v1 = Read(currentPosition + 1, modes[0]);
	NUM_TYPE v2 = Read(currentPosition + 2, modes[1]);
	Write(currentPosition + 3, v1 * v2, modes[2]);
}

void Computer::ProcessOpcode3() {
	auto modes = GetModes<1>();
	Write(currentPosition + 1, ioDriver->Read(), modes[0]);
}

void Computer::ProcessOpcode4() {
	auto modes = GetModes<1>();
	NUM_TYPE v = Read(currentPosition + 1, modes[0]);
	ioDriver->Write(v);
}

void Computer::ProcessOpcode5() {
	auto modes = GetModes<2>();
	NUM_TYPE v1 = Read(currentPosition + 1, modes[0]);
	NUM_TYPE v2 = Read(currentPosition + 2, modes[1]);
	if (v1 != 0) currentPosition = v2 - 3;
}

void Computer::ProcessOpcode6() {
	auto modes = GetModes<2>();
	NUM_TYPE v1 = Read(currentPosition + 1, modes[0]);
	NUM_TYPE v2 = Read(currentPosition + 2, modes[1]);
	if (v1 == 0) currentPosition = v2 - 3;
}

void Computer::ProcessOpcode7() {
	auto modes = GetModes<3>();
	NUM_TYPE v1 = Read(currentPosition + 1, modes[0]);
	NUM_TYPE v2 = Read(currentPosition + 2, modes[1]);
	Write(currentPosition + 3, v1 < v2 ? 1 : 0, modes[2]);
}

void Computer::ProcessOpcode8() {
	auto modes = GetModes<3>();
	NUM_TYPE v1 = Read(currentPosition + 1, modes[0]);
	NUM_TYPE v2 = Read(currentPosition + 2, modes[1]);
	Write(currentPosition + 3, v1 == v2 ? 1 : 0, modes[2]);
}

void Computer::ProcessOpcode9() {
	auto modes = GetModes<1>();
	NUM_TYPE v = Read(currentPosition + 1, modes[0]);
	relativeBase += v;
}
bool Computer::HasSufficientLength(int length) {
	return currentPosition + length - 1 < state_.size();
}


#define EXEC(CODE, LENGTH) \
	advance = LENGTH; \
	if (!HasSufficientLength(advance)) return -1; \
	ProcessOpcode##CODE(); \
	break

int Computer::Run() {

	while (currentPosition < state_.size()) {
		int opcode = state_[currentPosition] % 100;
		dlog << state_[currentPosition] << ", ";
		int advance = 0;
		switch (opcode) {
		case 1: EXEC(1, 4);
		case 2: EXEC(2, 4);
		case 3: EXEC(3, 2);
		case 4: EXEC(4, 2);
		case 5: EXEC(5, 3);
		case 6:	EXEC(6, 3);
		case 7: EXEC(7, 4);
		case 8: EXEC(8, 4);
		case 9: EXEC(9, 2);
		case 99:
			return state_[0];
		default:
			cout << "CRITICAL FAILURE " << currentPosition << endl;
			return -1;
		}
		dlog << currentPosition << endl;
		currentPosition += advance;
		if (ioDriver && ioDriver->Stop()) return -1;
	}
	return -1;
}
