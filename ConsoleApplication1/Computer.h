#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>

#define NUM_TYPE long long

namespace computer {

	class IODriver {
	public:
		virtual NUM_TYPE Read() = 0;
		virtual void Write(long long v) = 0;
		virtual bool Stop() {
			return false;
		}
	};

	class StandardIODriver : public IODriver {
	private:
		StandardIODriver();

	public:
		StandardIODriver(NUM_TYPE);
		std::vector<NUM_TYPE> outputs;
		NUM_TYPE input;
		NUM_TYPE prev = 0;
		NUM_TYPE Read() override;
		void Write(NUM_TYPE v) override;
	};

	class Computer {
	private:
		Computer();
		std::unordered_map<int, NUM_TYPE> state_;
		int currentPosition = 0;
		int relativeBase = 0;
		std::shared_ptr<IODriver> ioDriver;
		NUM_TYPE Read(int);
		NUM_TYPE Read(int, int mode);
		void Write(int, NUM_TYPE, int);
		bool HasSufficientLength(int);
		template<int N>
		std::array<int, N> GetModes() {
			std::array<int, N> modes;
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
		Computer(std::vector<NUM_TYPE> & initialState);
		Computer(std::vector<NUM_TYPE> & initialState, std::shared_ptr<IODriver> ioDriver);
		void RestoreState(NUM_TYPE, NUM_TYPE);
		int Run();
	};
}