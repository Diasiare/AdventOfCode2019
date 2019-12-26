#include "Day25.h"

#include "Computer.h"
#include "helpers.h"
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <deque>
#include <set>

namespace day25 {

	std::string take_everything = "south\ntake weather machine\nnorth\n"
		"west\nsouth\ntake festive hat\nsouth\ntake sand\nnorth\neast\ntake whirled peas\nwest\nnorth\n"
		"west\nnorth\ntake space heater\nsouth\neast\neast\n"
		"east\ntake mug\neast\nsouth\neast\nsouth\ntake easter egg\nnorth\nwest\n"
		"west\nsouth\nwest\ntake shell\nsouth\n";

	class Robot : public computer::IODriver {
	public:
		std::deque<std::string> command_queue;
		std::string input = "";
		int input_index = 0;
		std::vector<std::string> set_items = { "easter egg" ,"mug", "sand", "weather machine", "festive hat", "shell", "whirled peas", "space heater"};
		std::set<std::string> current_inventory;
		int set_iterator = 1;
		int subset_count;
		std::string output_since_last_read = "";
		bool bypased_door = false;

		Robot() {
			auto t = helpers::SplitBy(take_everything, "\n");
			command_queue.insert(command_queue.end(), t.begin(), t.end());
			subset_count = std::pow(2, set_items.size());
			current_inventory.insert(set_items.begin(), set_items.end());
		}

		void Write(NUM_TYPE output) override {
			output_since_last_read += char(output);
		}

		NUM_TYPE Read() override {
			std::cout << output_since_last_read;
			auto tmp = output_since_last_read;
			output_since_last_read = "";


			if (input_index == input.size()) {
				input_index = 0;
				input = "";
				if (!command_queue.empty()){
					input = command_queue.front();
					command_queue.pop_front();
				}
				else {
					std::getline(std::cin, input);
				}
				
				if (input.empty() && !bypased_door) {
					for (size_t i = 0; i < set_items.size(); i++)
					{
						std::string& item = set_items[i];
						bool include = (set_iterator & (1 << i)) > 0;
						if (include && current_inventory.count(item) == 0) {
							command_queue.push_back("take " + item);
							current_inventory.insert(item);
						} else if (!include && current_inventory.count(item) >= 0) {
							command_queue.push_back("drop " + item);
							current_inventory.erase(item);
						}
					}
					set_iterator++;
					command_queue.push_back("south");
					input = command_queue.front();
					command_queue.pop_front();
				}
				
				input += "\n";
				std::cout << input;
			}
			return input[input_index++];
		}
	};

	int Day25::Part1() {
		std::string input_file("Part25Input.txt");
		std::string input_string = helpers::ReadFile(input_file);
		std::vector<NUM_TYPE> code;
		for (auto s : helpers::SplitBy(input_string, ",")) {
			code.push_back(std::stoll(s));
		}
		
		std::shared_ptr<Robot> driver = std::make_shared<Robot>();
		computer::Computer c(code, driver);
		c.Run();

		std::cout << driver->output_since_last_read;

		return 0;
	}

}