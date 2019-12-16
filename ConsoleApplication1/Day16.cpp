#include "Day16.h"

#include <string>
#include <vector>
#include <iostream>
#include <array>
#include <unordered_map>

namespace day16 {

	std::vector<int> SplitString(std::string s) {
		std::vector<int> v;
		for (auto c : s) {
			v.push_back(c - '0');
		}
		return v;
	}

	int CalcMultiplier(long index, long sequenceLength) {

		long cycleLength = sequenceLength * 4;
		long calcBase = index + 1;
		long limited = calcBase % cycleLength;
		if (limited < cycleLength / 4) return 0;
		if (limited < cycleLength / 2) return 1;
		if (limited < cycleLength * 3 / 4) return 0;
		return -1;
	}

	std::vector<int> DoCycle(std::vector<int>& v) {
		std::vector<int> result;
		for (size_t i = 0; i < v.size(); i++)
		{
			int accum = 0;
			for (size_t i2 = 0; i2 < v.size(); i2++)
			{
				accum += CalcMultiplier(i2, i + 1) * v[i2];
			}
			result.push_back(std::abs(accum) % 10);
		}
		return result;
	}


	constexpr const char* input = "59790132880344516900093091154955597199863490073342910249565395038806135885706290664499164028251508292041959926849162473699550018653393834944216172810195882161876866188294352485183178740261279280213486011018791012560046012995409807741782162189252951939029564062935408459914894373210511494699108265315264830173403743547300700976944780004513514866386570658448247527151658945604790687693036691590606045331434271899594734825392560698221510565391059565109571638751133487824774572142934078485772422422132834305704887084146829228294925039109858598295988853017494057928948890390543290199918610303090142501490713145935617325806587528883833726972378426243439037";


	std::string Day16::Part1() {
		std::vector<int> v = SplitString(input);

		for (size_t i = 0; i < 100; i++) {
			v = DoCycle(v);
		}

		std::string out = "";
		for (auto a : v) {
			out += std::to_string(a);
		}

		return out.substr(0, 8);
	}


	/*
	std::vector<std::vector<int>> TranslationMatrix() {
		std::vector<std::vector<int>> matrix;

		for (size_t i = 0; i < P2_SIZE; i ++)
		{
			matrix.push_back(std::vector<int>());
			for (size_t i2 = 0; i2 < P2_SIZE; i2++)
			{
				matrix[i].push_back(CalcMultiplier(i2, i + 1));
			}
		}
		return matrix;
	}

	std::array<int, P2_SIZE> inline DoTranslation(const int* input, const std::vector<std::vector<int>>& matrix) {
		std::array<int, P2_SIZE> result;
		for (size_t i = 0; i < P2_SIZE; i++)
		{

			for (size_t i2 = i; i2 < P2_SIZE; i2++)
			{
				result[i] += matrix[i][i2] * input[i2];
			}
		}
		return result;

	}

	int ValueFor(int index, int layer, std::vector<int> & layerZero, std::unordered_map<int, std::unordered_map<int, int>> & cache) {
		if (layer == 0) return layerZero[index  % layerZero.size()];
		auto & currentLayer = cache[layer];
		auto v = currentLayer.find(index);
		if (v != currentLayer.end()) return (*v).second;

		int accum = 0;
		for (size_t i = index; i < P2_SIZE; i++)
		{
			int mult = CalcMultiplier(i, index + 1);
			if (mult == 0) continue;
			accum += ValueFor(i, layer - 1, layerZero, cache);
		}
		accum = std::abs(accum) % 10;
		currentLayer[index] = accum;
		return accum;
	}*/


	std::string Day16::Part2() {
		std::vector<int> initial = SplitString(input);
		std::vector<int> values;

		std::string sinput = input;
		int outIndex = std::stoi(sinput.substr(0, 7));


		for (size_t i = 0; i < initial.size() * 10000; i++)
		{
			values.push_back(initial[i % initial.size()]);
		}

		for (size_t i = 0; i < 100; i++)
		{
			int accum = 0;
			for (size_t i2 = values.size() -1 ; i2 >= outIndex; i2--)
			{
				accum = (accum + values[i2]) % 10;
				values[i2] = accum ;
			}
		}
		
		std::string out = "";
		for (int i = outIndex; i < outIndex + 8; i++) {
			int v = values[i];
			out += std::to_string(v);
		}

		return out;
	}


}
