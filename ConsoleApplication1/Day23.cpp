#include "Day23.h"

#include "Computer.h";
#include "helpers.h"
#include <vector>
#include <deque>
#include <thread>
#include <future>
#include <mutex>
#include <map>
#include <utility>
#include <memory>
#include <iostream>

namespace day23 {


	class NIC : public computer::IODriver {
	public:
		bool stopped = true;
		int own_adress;
		bool started = false;
		std::mutex lock;
		std::map<int, std::shared_ptr<NIC>> * nics;
		std::deque<int> remaining_reads;
		NUM_TYPE to = 0;
		NUM_TYPE x = 0;
		NUM_TYPE y = 0;
		int state = 0;

		NIC(int own_adress, std::map<int, std::shared_ptr<NIC>>* nics) : nics(nics), own_adress(own_adress) {
			state = 0;
		};

		virtual void PostMessage(NUM_TYPE x_val, NUM_TYPE y_val) {
			std::unique_lock<std::mutex> l(lock);
			remaining_reads.push_back(x_val);
			remaining_reads.push_back(y_val);
		}

		NUM_TYPE Read() override {
			std::unique_lock<std::mutex> l(lock);
			if (!started) {
				started = true;
				return own_adress;
			}
			if (remaining_reads.empty()) return -1;
			NUM_TYPE v = remaining_reads.front();
			remaining_reads.pop_front();
			return v;
		}

		void Write(NUM_TYPE output) override {
			if (state == 0) {
				to = output;
				if (to == 255) {
					std::cout << "TO 255" << std::endl;
				}
			}
			else if (state == 1) {
				x = output;
				if (to == 255) {
					std::cout << "TO 255" << std::endl;
				}
			}
			else if (state == 2) {
				y = output;
				if (to == 255) {
					std::cout << "TO 255" << std::endl;
				}
			}
			state++;
			if (state == 3) {
				state = 0;
				nics->at(to)->PostMessage(x, y);
			}
		}

		bool Stop() override {
			return stopped;
		}
	};

	class Adress255NIC : public NIC {
	public:
		std::promise<NUM_TYPE> y_promise;
		NUM_TYPE output = 0;
		bool has_output = false;

		Adress255NIC() : y_promise(std::promise<NUM_TYPE>()), NIC(255, nullptr) {};

		void PostMessage(NUM_TYPE x_val, NUM_TYPE y_val) override {
			std::unique_lock<std::mutex> l(lock);
			y_promise.set_value(y_val);
			output = y;
			has_output = true;
		}
	};

	std::string input_file = "Part23Input.txt";

	NUM_TYPE Day23::Part1() {
		std::string input_string = helpers::ReadFile(input_file);
		std::vector<NUM_TYPE> INPUT;
		for (auto s : helpers::SplitBy(input_string, ",")) {
			INPUT.push_back(std::stoll(s));
		}


		std::map<int, std::shared_ptr<NIC>> nics;
		for (int i = 0; i < 50; i++) {
			std::shared_ptr<NIC> nic = std::make_shared<NIC>(i, &nics);
			nics[i] = nic;
		}
		auto NIC255 = std::make_shared<Adress255NIC>();
		nics[255] = NIC255;


		/*
		std::vector<std::thread> threads;
		for (int i = 0; i < 50; i++) {
			threads.push_back(std::thread([ i, &nics]() {
					computer::Computer c(INPUT, nics[i]);
					c.Run();
				}));
		}

		int v = NIC255->y_promise.get_future().get();

		for (auto & p : nics) {
			p.second->stopped = true;
		}

		for (auto& t : threads) {
			t.join();
		}
		
		return v;
		*/

		std::vector<computer::Computer> machines;
		for (int i = 0; i < 50; i++) {
			machines.push_back(computer::Computer(INPUT, nics[i]));
		}
		int i = 0;
		while (!NIC255->has_output) {
			machines[i].Run();
			i = (i + 1) % 50;
		}

		return NIC255->output;
	}
}