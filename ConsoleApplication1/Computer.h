#pragma once

#include <vector>
#include <memory>

using namespace std;

class IODriver {
public:
	virtual int Read() { return 0; };
	virtual void Write(int) {};
};

class Computer {
private:
	vector<int> state_;
	int currentPosition = 0;
	shared_ptr<IODriver> ioDriver;
	int Read(int);
	int Read(int, int mode);
	void Write(int, int);
	bool HasSufficientLength(int);
	template<int N>
	array<int, N> GetModes() {
		array<int, N> modes;
		int base = state_[currentPosition] / 100;
		for (int i = 0; i < N; i++) {
			modes[i] = base % 10;
			base = base / 10;
		}
		return modes;
	};
	void ProcessOpcode1();
	void ProcessOpcode2();
	void ProcessOpcode3();
	void ProcessOpcode4();
	void ProcessOpcode5();
	void ProcessOpcode6();
	void ProcessOpcode7();
	void ProcessOpcode8();
public:
	Computer(vector<int> initialState);
	Computer(vector<int> initialState, shared_ptr<IODriver> ioDriver);
	void RestoreState(int, int);
	int Run();
};

