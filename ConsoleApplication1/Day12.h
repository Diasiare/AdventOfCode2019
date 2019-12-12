#pragma once

#include <array>

namespace day12 {

	class Moon
	{
	public:
		std::array<int, 3> position;
		std::array<int, 3> velocity;
		Moon(int, int, int);
		Moon(const Moon& other) : position(other.position) , velocity(other.velocity) {}
		bool operator==(const Moon& other) const;
		void ApplyGravity(const Moon& other);
		void Move();
		int Energy();
	};

	class State {
	public:
		std::array<Moon, 4> state;
		State(std::array<Moon, 4> initialState) : state(initialState) {}
		State(const State& other) : state(other.state) {};
		bool operator==(const State& a) const;
		State Next();
		int Energy();
		void ZeroAxis(int axis);
	};

	class Day12
	{
	public:
		static int Part1();
		static long long Part2();
	};

}