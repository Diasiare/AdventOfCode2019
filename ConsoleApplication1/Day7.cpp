#include "Day7.h"

#include "Computer.h";
#include <queue>
#include <set>
#include <vector>
#include <deque>
#include <future>
#include <mutex>

using namespace std;

class Part1IODriver : public IODriver {
public:
	deque<int> inputs;
	int output;

	Part1IODriver(deque<int> inputs) : inputs(inputs), output(0) {};

	int Read() {
		int v = inputs.front();
		inputs.pop_front();
		return v;
	}

	void Write(int v) {
		output = v;
	}
};

class Part2IODriver : public IODriver {
public:
	int setting;
	deque<int> inputs;
	promise<int> waitPromise;
	bool waiting = false;
	int lastOutput = 0;
	Part2IODriver *nextDriver;
	mutex lock;

	int Read() {
		auto f = GetInput();
		f.wait();
		return f.get();
	}

	void Write(int output) {
		lastOutput = output;
		nextDriver->AddInput(output);
	}

	void AddInput(int input) {
		unique_lock<mutex> l(this->lock);
		if (waiting) {
			waitPromise.set_value(input);
			waiting = false;
		}
		else inputs.push_back(input);
	}

	future<int> GetInput() {
		unique_lock<mutex> l(this->lock);
		if (inputs.empty()) {
			waiting = true;
			waitPromise = promise<int>();
			return waitPromise.get_future();
		}
		else {
			promise<int> p;
			int v = inputs.front();
			p.set_value(v);
			inputs.pop_front();
			return p.get_future();
		}
	}
};

int RecursiveSearch(const set<int> &phase_setting, int input_power) {
	if (phase_setting.empty()) return input_power;
	vector<int> code = { 3,8,1001,8,10,8,105,1,0,0,21,38,63,72,81,106,187,268,349,430,99999,3,9,101,5,9,9,1002,9,3,9,101,3,9,9,4,9,99,3,9,102,3,9,9,101,4,9,9,1002,9,2,9,1001,9,2,9,1002,9,4,9,4,9,99,3,9,1001,9,3,9,4,9,99,3,9,102,5,9,9,4,9,99,3,9,102,4,9,9,1001,9,2,9,1002,9,5,9,1001,9,2,9,102,3,9,9,4,9,99,3,9,1001,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,1,9,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,99,3,9,1001,9,2,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,1,9,9,4,9,3,9,102,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,1,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,1001,9,1,9,4,9,99,3,9,102,2,9,9,4,9,3,9,1001,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,99,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,1,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,101,1,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,99,3,9,102,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,101,1,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,1,9,9,4,9,3,9,102,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,1001,9,1,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,99 };
	set<int> clone = phase_setting;
	int maxPower = INT_MIN;
	for (int v : phase_setting) {
		deque<int> inputs = { v, input_power };
		clone.erase(v);
		shared_ptr<Part1IODriver> driver = make_shared<Part1IODriver>(inputs);
		Computer comp(code, driver);
		comp.Run();
		int output = RecursiveSearch(clone, driver->output);
		maxPower = max(maxPower, output);
		clone.insert(v);
	}
	return maxPower;
}

void DoRun(shared_ptr<IODriver> driver) {
	vector<int> code = { 3,8,1001,8,10,8,105,1,0,0,21,38,63,72,81,106,187,268,349,430,99999,3,9,101,5,9,9,1002,9,3,9,101,3,9,9,4,9,99,3,9,102,3,9,9,101,4,9,9,1002,9,2,9,1001,9,2,9,1002,9,4,9,4,9,99,3,9,1001,9,3,9,4,9,99,3,9,102,5,9,9,4,9,99,3,9,102,4,9,9,1001,9,2,9,1002,9,5,9,1001,9,2,9,102,3,9,9,4,9,99,3,9,1001,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,1,9,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,99,3,9,1001,9,2,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,1,9,9,4,9,3,9,102,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,1,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,1001,9,1,9,4,9,99,3,9,102,2,9,9,4,9,3,9,1001,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,99,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,1,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,101,1,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,99,3,9,102,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,101,1,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,1,9,9,4,9,3,9,102,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,1001,9,1,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,99 };
	Computer c(code, driver);
	c.Run();
}


int ConcurrentRecursiveSearch(const set<int>& remaning_settings, vector<int> current_settings) {
	if (remaning_settings.empty()) {
		vector<shared_ptr<Part2IODriver>> drivers;
		for (int setting : current_settings) {
			auto d = make_shared<Part2IODriver>();
			d->setting = setting;
			drivers.push_back(d);
			d->AddInput(setting);
		}

		for (int i = 0; i < drivers.size(); i++) {
			drivers[i]->nextDriver = drivers[(i + 1) % drivers.size()].get();
		}
		drivers[0]->AddInput(0);

		vector<thread> threads;
		for (auto driver : drivers) {
			threads.push_back(thread(DoRun, driver));
		}
		for (thread& t : threads) {
			t.join();
		}
		return drivers[drivers.size() - 1]->lastOutput;
	};
	set<int> clone = remaning_settings;
	int maxPower = INT_MIN;
	for (int v : remaning_settings) {
		current_settings.push_back(v);
		clone.erase(v);
		int output = ConcurrentRecursiveSearch(clone, current_settings);
		maxPower = max(maxPower, output);
		clone.insert(v);
		current_settings.pop_back();
	}
	return maxPower;

}

int Day7::Part1() {
	set<int> inputs = { 0,1,2,3,4 };
	int input_power = 0;
	return RecursiveSearch(inputs, input_power);
}

int Day7::Part2() {
	set<int> inputs = { 5, 6, 7, 8, 9 };
	return ConcurrentRecursiveSearch(inputs, vector<int>());
}