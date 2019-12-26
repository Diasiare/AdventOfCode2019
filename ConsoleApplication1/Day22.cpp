#include "Day22.h"


#include "helpers.h"
#include <array>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <functional>
#include "InfInt.h"



namespace day22 {
	constexpr size_t DECK_SIZE = 10007;

	void DealWithIncrement(std::array<int, DECK_SIZE>* from, std::array<int, DECK_SIZE>* to, int increment) {
		for (int i = 0; i < from->size(); i++) {
			to->at((i * increment) % from->size()) = from->at(i);
		}
	}

	void DealInto(std::array<int, DECK_SIZE>* from, std::array<int, DECK_SIZE>* to) {
		auto to_it = to->rbegin();
		for (auto it = from->begin(); it != from->end(); it++) {
			*to_it = *it;
			to_it++;
		}
	}

	void Cut(std::array<int, DECK_SIZE>* from, std::array<int, DECK_SIZE>* to, int num) {
		int size = from->size();
		int start_copy_at = num < 0 ? size - abs(num) : num;
		for (int i = 0; i < size; i++) {
			to->at(i) = from->at((start_copy_at++) % size);
		}
	}

	void ExecLine(std::string& line, std::array<int, DECK_SIZE>* from, std::array<int, DECK_SIZE>* to) {
		if (line == "deal into new stack") {
			DealInto(from, to);
		}
		else {
			std::vector<std::string> parts = helpers::SplitBy(line, " ");
			int num = std::stoi(parts[parts.size() - 1]);
			if (parts[0] == "cut") {
				Cut(from, to, num);
			}
			else {
				DealWithIncrement(from, to, num);
			}
		}
	}

	int Day22::Part1() {
		std::string input_file = "Part22Input.txt";
		std::string input = helpers::ReadFile(input_file);
		std::vector<std::string> lines = helpers::SplitBy(input, "\n");


		std::array<int, DECK_SIZE>* from = new std::array<int, DECK_SIZE>();
		std::array<int, DECK_SIZE>* to = new std::array<int, DECK_SIZE>();
		for (int i = 0; i < from->size(); i++) {
			from->at(i) = i;
		}
		
		for (std::string& line : lines) {
			ExecLine(line, from, to);
			std::swap(from, to);
		}

		for (int i = 0; i < from->size(); i++) {
			if (from->at(i) == 2019) return i;
		}

		return -1;
	}

	InfInt gcdExtended(InfInt a, InfInt b, InfInt* x, InfInt* y);

	// Function to find modulo inverse of b. It returns 
	// -1 when inverse doesn't 
	InfInt modInverse(InfInt b, InfInt m)
	{
		InfInt x, y; // used in extended GCD algorithm 
		InfInt g = gcdExtended(b, m, &x, &y);

		// Return -1 if b and m are not co-prime 
		if (g != 1)
			return -1;

		// m is added to handle negative x 
		return (x % m + m) % m;
	}

	// Function to compute a/b under modlo m 
	InfInt modDivide(InfInt a, InfInt b, InfInt m)
	{
		a = a % m;
		InfInt inv = modInverse(b, m);
		return (inv * a) % m;
	}

	// C function for extended Euclidean Algorithm (used to 
	// find modular inverse. 
	InfInt gcdExtended(InfInt a, InfInt b, InfInt* x, InfInt* y)
	{
		// Base Case 
		if (a == 0)
		{
			*x = 0, * y = 1;
			return b;
		}

		InfInt x1, y1; // To store results of recursive call 
		InfInt gcd = gcdExtended(b % a, a, &x1, &y1);

		// Update x and y using results of recursive 
		// call 
		*x = y1 - (b / a) * x1;
		*y = x1;

		return gcd;
	}

	long long Day22::Part2() {
		InfInt goal_index = 101741582076661;
		InfInt deck_size = 119315717514047;

		std::string input_file = "Part22Input.txt";
		std::string input = helpers::ReadFile(input_file);
		std::vector<std::string> lines = helpers::SplitBy(input, "\n");


		InfInt start_index = 2020;
		InfInt current_index = start_index;



		/*
		std::vector<std::function<long long(long long)>> operations;

		for (std::string line : lines) {
			std::function<long long(long long)> f;
			if (line == "deal into new stack") {
				f = [deck_size](long long current_index) {
					return ((deck_size - 1) - current_index);
				};
			}
			else {
				std::vector<std::string> parts = helpers::SplitBy(line, " ");
				int num = std::stoi(parts[parts.size() - 1]);
				if (parts[0] == "cut") {
					f = [deck_size, num](long long current_index) {
						current_index = current_index - num;
						current_index = deck_size + current_index;
						return current_index % deck_size;
					};
				}
				else {
					f = [deck_size, num](long long current_index) {
						return (current_index * num) % deck_size;
					}; 
				}
			}
			operations.push_back(f);
		}*/

		InfInt multiplier = 1;
		InfInt constant = 0;


		for (std::string line : lines) {
			if (line == "deal into new stack") {
				multiplier *= -1;
				constant =  (constant * -1) + deck_size - 1;
			}
			else {
				std::vector<std::string> parts = helpers::SplitBy(line, " ");
				int num = std::stoi(parts[parts.size() - 1]);
				if (parts[0] == "cut") {
					constant += deck_size - num;
				}
				else {
					multiplier *= num;
					constant *= num;
				}
			}
			multiplier = (multiplier + deck_size) % deck_size;
			constant = (constant + deck_size) % deck_size;
		}

		std::vector<InfInt> multipliers = { multiplier };
		std::vector<InfInt> constants = { constant };
		std::vector<InfInt> times = { 1 };

		for (InfInt power = 2; power <= goal_index * 2; power *= 2) {
			InfInt p_mult = multipliers.back();
			InfInt p_constant = constants.back();
			multipliers.push_back((p_mult * p_mult) % deck_size);
			constants.push_back((p_constant + (p_mult * p_constant)) % deck_size);
			times.push_back(power);
		}

		InfInt rem = goal_index;
		multiplier = 1;
		constant = 0;
		while (rem > 0) {
			int i;
			for (i = 0; times[i] <= rem; i++);
			i--;
			multiplier = (multiplier * multipliers[i]) % deck_size;
			constant = (constant * multipliers[i] + constants[i]) % deck_size;
			rem -= times[i];
		}
		
		InfInt result = (start_index - constant) % deck_size;
		if (result < 0) result += deck_size;
		result = modDivide(result, multiplier, deck_size);
		if (result < 0) result += deck_size;

		
		return result.toLongLong();
	}
}