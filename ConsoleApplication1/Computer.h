#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>

#define NUM_TYPE long long

using namespace std;

namespace computer {

	class IODriver {
	public:
		virtual NUM_TYPE Read() = 0;
		virtual void Write(long long v) = 0;
	};

	class StandardIODriver : public IODriver {
	private:
		StandardIODriver();

	public:
		StandardIODriver(NUM_TYPE);
		vector<NUM_TYPE> outputs;
		NUM_TYPE input;
		NUM_TYPE prev = 0;
		NUM_TYPE Read() override;
		void Write(NUM_TYPE v) override;
	};

	class Computer {
	private:
		Computer();
		unordered_map<int, NUM_TYPE> state_;
		int currentPosition = 0;
		int relativeBase = 0;
		shared_ptr<IODriver> ioDriver;
		NUM_TYPE Read(int);
		NUM_TYPE Read(int, int mode);
		void Write(int, NUM_TYPE, int);
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
		void ProcessOpcode9();
	public:
		Computer(vector<NUM_TYPE> initialState);
		Computer(vector<NUM_TYPE> initialState, shared_ptr<IODriver> ioDriver);
		void RestoreState(NUM_TYPE, NUM_TYPE);
		int Run();
	};
}