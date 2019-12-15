#include "Day14.h"

#include <vector>
#include <utility>
#include <string>;
#include <iostream>
#include <map>
#include "helpers.h"


namespace day14 {

	class Reaction {
	public:
		std::vector<std::pair<std::string, int_fast64_t>> ingredients;
		std::string produces;
		int quantity;

		Reaction(std::string result, int quantity, std::vector<std::pair<std::string, int_fast64_t>> ingredients) :
			ingredients(ingredients), produces(result), quantity(quantity) {};

		std::string ToString() {
			std::string result = "";
			for (auto& i : ingredients) {
				result += i.first + " " + std::to_string(i.second) + " ,";
			}
			result += " => " + produces + " " + std::to_string(quantity);
			return result;
		}

		int_fast64_t Produce(int_fast64_t quantityToMake, std::map<std::string, Reaction> & reactions, std::map<std::string, int_fast64_t> & store) {
			// Materials are allready there
			if (store[produces] >= quantityToMake) return 0;
			
			int_fast64_t toProduce = quantityToMake - store[produces];
			int_fast64_t reactionCount = toProduce / quantity;
			if (toProduce % quantity != 0) reactionCount++;

			int_fast64_t oreUsed = 0;
			for (auto& c : ingredients) {
				if (c.first == "ORE") oreUsed += c.second * reactionCount;
				else {
					oreUsed += reactions.find(c.first)->second.Produce(c.second * reactionCount, reactions, store);
					store[c.first] -= c.second * reactionCount;
				};
			}

			store[produces] += reactionCount * quantity;
			return oreUsed;
		}

	};


	std::map<std::string, Reaction> ParseInput() {
		std::string inFile = "Part14Input.txt";
		std::string input = helpers::ReadFile(inFile);
		std::map<std::string, Reaction> out;

		for (auto& line : helpers::SplitBy(input, "\n")) {
			auto comp = helpers::SplitBy(line, "=>");
			auto results = helpers::SplitBy(comp[1], " ");
			auto material = results[1];
			int amount = std::stoi(results[0]);
			std::vector<std::pair<std::string, int_fast64_t>> rcomp;
			for (std::string& component : helpers::SplitBy(comp[0], ",")) {
				auto a = helpers::SplitBy(component, " ");
				rcomp.push_back(std::make_pair(a[1], std::stoi(a[0])));
			}
			out.emplace(material, Reaction(material, amount, rcomp));
		}
		return out;
	};


	int Day14::Part1() {
		auto input = ParseInput();
		std::map<std::string, int_fast64_t> materialStore;


		return input.find("FUEL")->second.Produce(1, input, materialStore);
	}

	int Day14::Part2() {
		auto input = ParseInput();
		std::map<std::string, int_fast64_t> materialStore;

		Reaction& fuelReaction = input.find("FUEL")->second;

		int_fast64_t ORE_CAP = 1000000000000;
		int_fast64_t oreUsed = fuelReaction.Produce(1, input, materialStore);
		int maxCost = oreUsed;
		int_fast64_t fuelProduced = 0;

		while (oreUsed <= ORE_CAP) {
			fuelProduced += materialStore["FUEL"];
			materialStore["FUEL"] = 0;
			int_fast64_t toProduce = ((ORE_CAP - oreUsed) / maxCost);
			if (toProduce < 1) toProduce = 1;
			oreUsed += fuelReaction.Produce(toProduce, input, materialStore);
		}
		
		return fuelProduced;
	}
}