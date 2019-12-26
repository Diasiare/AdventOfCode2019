#include "helpers.h"

#include <string>
#include <fstream>
#include <vector>
#include <sstream>

namespace helpers {

	std::vector<std::string> SplitBy(std::string& input, std::string by) {
		std::vector<std::string> output;
		size_t nextStart = 0;
		size_t pos = input.find(by);
		while (pos != std::string::npos) {
			if (pos != nextStart) output.push_back(input.substr(nextStart, pos - nextStart));
			nextStart = pos + by.size();
			pos = input.find(by, nextStart + by.size());
		};
		output.push_back(input.substr(nextStart, input.size() - nextStart));
		return output;
	};

	std::string ReadFile(std::string& name) {
		std::ifstream t(name);
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	}

	int countSetBits(int n)
	{
		// base case 
		if (n == 0)
			return 0;
		else
			return 1 + countSetBits(n & (n - 1));
	}
}