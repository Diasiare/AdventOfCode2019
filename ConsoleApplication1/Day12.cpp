#include "Day12.h"

#include <unordered_set>
#include <unordered_map>
#include <array>
#include <iostream>
#include "helpers.h"

namespace std {
	template <>
	struct hash<day12::Moon> {
		std::size_t operator()(const day12::Moon& m) const {
			using std::hash;
			hash<int> h;

			return h(m.position[0]) ^ h(m.position[1]) ^ h(m.position[2]) ^ h(m.velocity[0]) ^ h(m.velocity[1]) ^ h(m.velocity[2]);
		}
	};

	template <>
	struct hash<day12::State> {
		std::size_t operator()(const day12::State &m) const {
			using std::hash;
			hash<day12::Moon> h;

			return h(m.state[0]) ^ h(m.state[1]) ^ h(m.state[2]) ^ h(m.state[3]);
		}
	};
}

namespace day12 {

	Moon::Moon(int x, int y, int z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
		velocity = { 0, 0, 0 };
	}

	bool Moon::operator== (const Moon& other) const {
		return position == other.position && velocity == other.velocity;
	};

	void Moon::ApplyGravity(const Moon& other) {
		for (int i = 0; i < velocity.size(); i++) {
			if (position[i] == other.position[i]) continue;
			velocity[i] += position[i] < other.position[i] ? 1 : -1;
		}
	}

	void Moon::Move() {
		for (int i = 0; i < velocity.size(); i++) {
			position[i] += velocity[i];
		}
	}

	int Moon::Energy() {
		int ep = 0;
		for (int p : position) {
			ep += std::abs(p);
		}
		int ev = 0;
		for (int v : velocity) {
			ev += std::abs(v);
		}

		return ep * ev;
	}


	State State::Next() {
		State n(*this);
		for (int i = 0; i < 4; ++i) {
			for (int i2 = 0; i2 < 4; ++i2) {
				n.state[i].ApplyGravity(state[i2]);
			}
		}

		for (int i = 0; i < 4; ++i) {
			n.state[i].Move();
		}
		return n;
	}

	int State::Energy() {
		int e = 0;
		for (Moon& m : state) {
			e += m.Energy();
		}
		return e;
	}

	bool State::operator==(const State& a) const {
		return state[0] == a.state[0] && state[2] == a.state[2] && state[1] == a.state[1];
	}

	void State::ZeroAxis(int axis) {
		for (Moon& m : state) {
			for (size_t i = 0; i < 3; i++)
			{
				if (i == axis) continue;
				m.velocity[i] = 0;
				m.position[i] = 0;
			}
		}
	}

	int Day12::Part1() {
		std::array<Moon, 4> moons = /*{
			Moon(-1, 0, 2),
			Moon(2, -10, -7),
			Moon(4, -8, 8),
			Moon(3, 5, -1)
		};*/{
			Moon(-4, -14, 8),
			Moon(1, -8, 10),
			Moon(-15, 2, 1),
			Moon(-17, -17, 16)
		};
		State state(moons);

		for (int i = 0; i < 1000; i++) {
			state = state.Next();
		}
		return state.Energy();
	}


	void FindFirstLoop(State state, int axis, int* lowerBound, int* upperBound) {
		long long step = 0;
		std::hash<State> hash;
		std::unordered_map<State, int, decltype(hash)> set(10000, hash);

		state.ZeroAxis(axis);

		while (set.count(state) == 0) {
			set[state] = step;
			state = state.Next();
			step += 1;
		}
		*lowerBound = set[state];
		*upperBound = step;
	}

	long long Day12::Part2() {
		long long step = 0;
		std::hash<State> hash;
		
		std::array<Moon, 4> moons = /*{
	Moon(-1, 0, 2),
	Moon(2, -10, -7),
	Moon(4, -8, 8),
	Moon(3, 5, -1)
}; */{
	Moon(-4, -14, 8),
	Moon(1, -8, 10),
	Moon(-15, 2, 1),
	Moon(-17, -17, 16)
		}; 

		State state(moons);
		std::array<int, 3> lowerBounds;
		std::array<int, 3> upperBounds;
		for (int i = 0; i < 3; i++) {
			int lowerBound = 0;
			int upperBound = 0;
			FindFirstLoop(state, i, &lowerBound, &upperBound);
			lowerBounds[i] = lowerBound;
			upperBounds[i] = upperBound;
		}

		return helpers::LCM<long long>(upperBounds[0], helpers::LCM<long long>(upperBounds[1], upperBounds[2]));

	}

}