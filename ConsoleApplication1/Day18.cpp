#include "Day18.h"

#include "helpers.h"

#include <map>
#include <utility>
#include <string>
#include <set>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <functional>
#include <iostream>
#include <array>

namespace day18 {

	class Node;
	typedef std::vector<std::vector<Node*>> Map;

	int nextId = 0;

	enum class NodeType {
		Intersection,
		Key,
		Gate,
		Wall,
		Path,
	};

	class Node {
	public:
		std::map<Node *, int> neighbours;
		std::pair<int, int> position;
		NodeType type;
		char letter;

		Node(NodeType type) : type(type) {}

		void AddNeighbour(Node * other, int distance) {
			neighbours[other] = distance;
			other->neighbours[this] = distance;
		}

		void Cleanup() {
			// Do not clean up important nodes
			if (type == NodeType::Gate || type == NodeType::Key) return;
			if (neighbours.size() == 0 || neighbours.size() > 2) return;
			if (neighbours.size() == 1) {
				auto other = neighbours.begin()->first;
				neighbours.clear();
				other->neighbours.erase(this);
				other->Cleanup();
				return;
			}
			auto it = neighbours.begin();
			auto first = *it;
			auto second = *(++it);

			first.first->AddNeighbour(second.first, first.second + second.second);
			first.first->neighbours.erase(this);
			second.first->neighbours.erase(this);

		}
	};

	void MaybeAdd(Node* c, Node* o) {
		if (o->type == NodeType::Wall) return;
		c->AddNeighbour(o, 1);
	}

	void ConstructGraph(Map & map) {
		for (size_t i = 0; i < map.size(); i++)
		{
			for (int i2 = 0; i2 < map[i].size(); i2++) {
				auto c = map[i][i2];
				if (c->type == NodeType::Wall) continue;
				MaybeAdd(c, map[i - 1][i2]);
				MaybeAdd(c, map[i][i2 + 1]);
				MaybeAdd(c, map[i + 1][i2]);
				MaybeAdd(c, map[i][i2 - 1]);
			}
		}
	}

	void PruneGraph(Map& map) {

		for (auto r : map) {
			for (auto n : r) {
				if (n->neighbours.size() == 1) n->Cleanup();
			}
		}

		for (auto r : map) {
			for (auto n : r) {
				n->Cleanup();
			}
		}
	}

	int Distance(Node* start, Node* end, std::unordered_set<char> & closedGates) {
		std::set<Node*> visited;
		typedef std::pair<int, Node*> DNode;

		auto comp = [](const DNode& a, const DNode& b) {
			if (a.first == b.first) return a.second < b.second;
			return a.first > b.first;
		};
		std::priority_queue<DNode, std::vector<DNode>, std::function<bool(const DNode&, const DNode&)>> search(comp);
		search.push(std::make_pair(0, start));

		while (!search.empty()) {
			auto next = search.top();
			Node* n = next.second;
			int current_distance = next.first;
			if (n == end) return current_distance;
			search.pop();
			if (visited.count(n) > 0) continue;
			visited.insert(n);

			if (n->type == NodeType::Gate && closedGates.count(n->letter) > 0) continue;

			for (auto next : n->neighbours) {
				if (visited.count(next.first) > 0) continue;
				search.push(std::make_pair(current_distance + next.second, next.first));
			}
		}
		return -1;


	}

	typedef std::set<Node*> NodeSet;

	class SearchNode {
	public:
		Node* position;
		std::unordered_set<char> closed_gates;
		NodeSet remaningKeys;
		int distance;

		SearchNode(Node* position ,
		std::unordered_set<char> & closed_gates,
		NodeSet & remaningKeys,
		int distance) : position(position), closed_gates(closed_gates), remaningKeys(remaningKeys), distance(distance) {}
	};

	inline bool Wasted(const SearchNode & n, std::map<std::pair<NodeSet,Node*>, int> & minimums) {
		std::pair<NodeSet, Node*> p = std::make_pair(n.remaningKeys, n.position);
		auto it = minimums.find(p);
		if (it != minimums.end() && it->second <= n.distance) {
			return true;
		}
		minimums[p] = n.distance;
		return false;
	}

	int SearchBy(SearchNode start) {
		int minKeyDist = INT_MAX;
		std::unordered_set<char> empty;
		for (auto k1 : start.remaningKeys) {
			for (auto k2 : start.remaningKeys) {
				int dist = Distance(k1, k2, empty);
				if (dist <= 0) continue;
				minKeyDist = std::min(minKeyDist, dist);
			}
		}

		auto comp = [minKeyDist](const SearchNode& a, const SearchNode& b) {

			int a_score = a.distance + a.remaningKeys.size() * minKeyDist;
			int b_score = b.distance + b.remaningKeys.size() * minKeyDist;
			if (a_score != b_score) return a_score > b_score;
			if (a.remaningKeys.size() != b.remaningKeys.size()) return a.remaningKeys.size() > b.remaningKeys.size();
			return a.position > b.position;
		};
		std::priority_queue<SearchNode, std::vector<SearchNode>, std::function<bool(const SearchNode&, const SearchNode&)>> search(comp);
		search.push(start);
	
		std::map<std::pair<NodeSet, Node*>, int> minimums;

		int minKeysRemaning = INT_MAX;
		while (!search.empty()) {
			SearchNode node = search.top();
			search.pop();
			if (node.remaningKeys.size() == 0) return node.distance;
			if (node.remaningKeys.size() < minKeysRemaning) {
				minKeysRemaning = node.remaningKeys.size();
				std::cout << minKeysRemaning << std::endl;
			}
			
			for (auto nextKey : node.remaningKeys) {
				int dist = Distance(node.position, nextKey, node.closed_gates);
				if (dist < 0) continue;
				SearchNode new_node(nextKey, node.closed_gates, node.remaningKeys, node.distance + dist);
				new_node.closed_gates.erase(nextKey->letter);
				new_node.remaningKeys.erase(nextKey);
				if (Wasted(new_node, minimums)) continue;
				search.push(new_node);
			}
		}
		return -1;
	}


	class BFSSearchNode {
	public:
		Node* position;
		int open_gates;
		int distance;

		BFSSearchNode(Node* start) : position(start), open_gates(0), distance(0) {}
		BFSSearchNode(Node* node, int open_gates) : position(node), open_gates(open_gates), distance(0) {}
		BFSSearchNode(BFSSearchNode& old, Node* new_position) {
			distance = old.distance + 1;
			position = new_position;
			open_gates = old.open_gates;
			if (new_position->type == NodeType::Key) open_gates |= 1 << (new_position->letter - 'A');
		}
	};

	const auto less = std::less<Node*>();

	bool CompWithoutDist(const BFSSearchNode& a, const BFSSearchNode& b) {
		if (a.open_gates != b.open_gates) return a.open_gates < b.open_gates;
		return less(a.position,  b.position);
	}

	int BFSFrom(BFSSearchNode start, std::set<Node *> keys) {
		int confirmation_pattern = 0;
		for (auto gate : keys) {
			confirmation_pattern |= 1 << (gate->letter - 'A');
		}

		std::deque<BFSSearchNode> queue = { start };
		std::map<BFSSearchNode, int, decltype(&CompWithoutDist)> bestPath(&CompWithoutDist);
		while (!queue.empty()) {
			BFSSearchNode node = queue.front();
			queue.pop_front();

			if ((node.open_gates ^ confirmation_pattern) == 0) return node.distance;
			auto bestIt = bestPath.find(node);
			if (bestIt == bestPath.end() || bestIt->second > node.distance) bestPath[node] = node.distance;
			else continue;

			for (auto & n : node.position->neighbours) {
				if (n.first->type == NodeType::Gate && ((1 << (n.first->letter - 'A')) & node.open_gates) == 0) continue;
				queue.push_back(BFSSearchNode(node, n.first));
			}
		}

		return -1;
	}

	/*
	int FindRemaningKeys(Node * start, std::set<char> & closed_gates, std::set<Node *> & remaningKeys, int prev_layer_dist, int stop_at) {
		if (remaningKeys.size() == 0) return 0;


		int best = INT_MAX;
		for (auto next : remaningKeys) {
			int dist = Distance(start, next, closed_gates);
			if (dist < 0 || stop_at < prev_layer_dist + dist) continue;

			std::set<Node*> nrem = remaningKeys;
			nrem.erase(next);
			closed_gates.erase(next->letter);

			int n_stop_at = best == INT_MAX ? stop_at : std::min(stop_at, best + prev_layer_dist);

			int l_dist = FindRemaningKeys(next, closed_gates, nrem, prev_layer_dist + dist, n_stop_at);
			int total_dist = l_dist == INT_MAX? l_dist : l_dist + dist;
			closed_gates.insert(next->letter);
			best = std::min(best, total_dist);
		}

		return best;
	}*/

	void RemoveNonEssentialNodes(NodeSet start, std::unordered_set<Node*> gates, NodeSet keys) {
		std::unordered_set<char> closed_gates;
		for (auto gate : gates) closed_gates.insert(gate->letter);
		
		std::vector<Node*> all;
		all.insert(all.end(), start.begin(), start.end());
		all.insert(all.end(), gates.begin(), gates.end());
		all.insert(all.end(), keys.begin(), keys.end());

		for (auto a : all) {
			for (auto b : all) {
				int dist = Distance(a, b, closed_gates);
				if (dist <= 0) continue;
				a->AddNeighbour(b, dist);
			}
		}

		for (auto n : all) {
			auto it = n->neighbours.begin();
			while (it != n->neighbours.end()) {
				auto other = it->first;
				if (other->type == NodeType::Path && start.count(other) == 0) it = n->neighbours.erase(it);
				else it++;
			}
		}

	}

	int Day18::Part1() {
		std::string fileName = "Part18Input.txt";
		std::string input = helpers::ReadFile(fileName);
		auto rows = helpers::SplitBy(input, "\n");

		Map map;

		std::pair<int, int> startingPosition;

		std::unordered_set<Node*> gates;
		NodeSet keys;
		Node* start = nullptr;

		int rowIndex = 0;
		int column = 0;
		for (auto& row : rows) {
			std::vector<Node *> v;
			for (char c : row) {
				if (c == '@') {
					auto n = new Node(NodeType::Path);
					start = n;
					v.push_back(n);
					startingPosition = std::make_pair(rowIndex, column);
				}
				else if ('a' <= c && c <= 'z') {
					Node * n= new Node(NodeType::Key);
					n->letter = std::toupper(c);
					keys.insert(n);
					v.push_back(n);
				} else if ('A' <= c && c <= 'Z') {
					Node  * n = new Node(NodeType::Gate);
					n->letter = c;
					gates.insert(n);
					v.push_back(n);
				}
				else if (c == '#') {
					v.push_back(new Node(NodeType::Wall));
				}
				else if (c == '.') {
					v.push_back(new Node(NodeType::Path));
				}
				v.back()->position = std::make_pair(rowIndex, column);
				column++;
			}
			map.push_back(v);
			rowIndex++;
			column = 0;
		}

		ConstructGraph(map);
		//RemoveNonEssentialNodes(start, gates, keys);

		std::unordered_set<char> closed_gates;
		for (auto gate : gates) closed_gates.insert(gate->letter);

		if (start == nullptr) return -1;

		SearchNode start_node(start, closed_gates, keys, 0);

		return BFSFrom(BFSSearchNode(start), keys);
	}


	class P2BFSSearchNode {
	public:
		std::array<Node*, 4> position;
		int open_gates;
		std::array<int, 4> distance = {0, 0, 0, 0};
		//std::vector<std::array<std::pair<int, int>, 4>> traces;

		P2BFSSearchNode(std::array<Node*, 4>& starts) : position(starts), open_gates(0) {
			/*std::array<std::pair<int, int>, 4> startTrace;
			auto it = startTrace.begin();
			for (auto n : starts) {
				*it = n->position;
				it++;
			}
			traces.push_back(startTrace);*/
		}
		P2BFSSearchNode(P2BFSSearchNode& old, Node* const new_position, int index) {
			//traces = old.traces;
			//std::array<std::pair<int, int>, 4> tracePoint = traces.back();
			//tracePoint[index] = (new_position->position);
			//traces.push_back(tracePoint);
			distance = old.distance;
			distance[index] += 1;
			position = old.position;
			position[index] = new_position;
			open_gates = old.open_gates;
			if (new_position->type == NodeType::Key) {
				open_gates |= 1 << (new_position->letter - 'A');
			}
		}

		int Distance() {
			return distance[0] + distance[1] + distance[2] + distance[3];
		}
	};

	bool P2CompWithoutDist(const P2BFSSearchNode& a, const P2BFSSearchNode& b) {
		if (a.open_gates != b.open_gates) return a.open_gates < b.open_gates;
		for (size_t i = 0; i < a.position.size(); i++)
		{
			if (a.position[i] != b.position[i]) return less(a.position[i], b.position[i]);
		}
		return false;
	}

	void PrintMap(Map& map, std::array<std::pair<int, int>, 4> & poses) {
		std::string s = "";
		for (size_t i = 0; i < map.size(); i++) {
			for (size_t i2 = 0; i2 < map[i].size(); i2++)
			{
				bool found = false;
				for (auto pos : poses) {
					if (pos.first == i && pos.second == i2) {
						s += "@";
						found = true;
						break;
					}
				}
				if (found) continue;
				switch (map[i][i2]->type)
				{
				case NodeType::Path:
					s += ".";
					break;
				case NodeType::Wall:
					s += "#";
					break;
				case NodeType::Gate:
					s += map[i][i2]->letter;
					break;
				case NodeType::Key:
					s += (char)std::tolower(map[i][i2]->letter);
					break;
				default:
					break;
				}
			}
			s += "\n";
		}
		std::cout << s;
	}

	int P2BFSFrom(P2BFSSearchNode start, std::set<Node*> keys, Map & map) {
		int confirmation_pattern = 0;
		for (auto gate : keys) {
			confirmation_pattern |= 1 << (gate->letter - 'A');
		}

		std::deque<P2BFSSearchNode> queue = { start };
		std::array<std::map<BFSSearchNode, int, decltype(&CompWithoutDist)>, 4> bestPaths = {
			std::map<BFSSearchNode, int, decltype(&CompWithoutDist)>(&CompWithoutDist),
			std::map<BFSSearchNode, int, decltype(&CompWithoutDist)>(&CompWithoutDist),
			std::map<BFSSearchNode, int, decltype(&CompWithoutDist)>(&CompWithoutDist),
			std::map<BFSSearchNode, int, decltype(&CompWithoutDist)>(&CompWithoutDist),
		};

		std::map<P2BFSSearchNode, int, decltype(&P2CompWithoutDist)> bestPath(&P2CompWithoutDist);

		int setCount = -1;
		while (!queue.empty()) {
			P2BFSSearchNode node = queue.front();
			queue.pop_front();
			
			int countThis = helpers::countSetBits(node.open_gates);
			if (countThis > setCount) {
				setCount = countThis;
				std::cout << countThis << std::endl;
			}
			

			auto bestIt = bestPath.find(node);
			if (bestIt == bestPath.end() || bestIt->second > node.Distance()) bestPath[node] = node.Distance();
			else continue;


			if ((node.open_gates ^ confirmation_pattern) == 0) {
				/*for (size_t i = 0; i < node.traces.size(); i++)
				{
					std::cout << std::endl << "INDEX: " << i << std::endl;
					PrintMap(map, node.traces[i]);
				}*/
				return node.distance[0] + node.distance[1] + node.distance[2] + node.distance[3];
			}
			
			for (int i = 0; i < node.position.size(); i++)
			{
				/*
				BFSSearchNode tmp(node.position[i], node.open_gates);
				auto bestIt = bestPaths[i].find(tmp);
				if (bestIt == bestPaths[i].end() || bestIt->second > node.distance[i]) bestPaths[i][tmp] = node.distance[i];
				else continue;*/

				for (auto& n : node.position[i]->neighbours) {
					if (n.first->type == NodeType::Gate && ((1 << (n.first->letter - 'A')) & node.open_gates) == 0) continue;
					queue.push_back(P2BFSSearchNode(node, n.first, i));
				}
			}
		}

		return -1;
	}


	class P2AStarSearchNode {
	public:
		std::array<Node*, 4> position;
		std::unordered_set<char> remaningKeys;
		std::array<int, 4> distance = { 0, 0, 0, 0 };
		//std::vector<std::array<std::pair<int, int>, 4>> traces;

		P2AStarSearchNode(std::array<Node*, 4>& starts, NodeSet & keys) : position(starts) {
			/*std::array<std::pair<int, int>, 4> startTrace;
			auto it = startTrace.begin();
			for (auto n : starts) {
				*it = n->position;
				it++;
			}
			traces.push_back(startTrace);*/

			for (auto gate : keys) {
				remaningKeys.insert(gate->letter);
			}
		}
		P2AStarSearchNode(P2AStarSearchNode& old, Node* const new_position, int index) {
			//traces = old.traces;
			//std::array<std::pair<int, int>, 4> tracePoint = traces.back();
			//tracePoint[index] = (new_position->position);
			//traces.push_back(tracePoint);
			distance = old.distance;
			distance[index] += old.position[index]->neighbours[new_position];
			position = old.position;
			position[index] = new_position;
			remaningKeys = old.remaningKeys;
			if (new_position->type == NodeType::Key) remaningKeys.erase(new_position->letter);
		}

		int Distance() const {
			return distance[0] + distance[1] + distance[2] + distance[3];
		}
	};



	bool P2AStarComp(const P2AStarSearchNode& a, const P2AStarSearchNode& b) {
		if (a.Distance() != b.Distance()) return a.Distance() > b.Distance();
			if (a.remaningKeys.size() != b.remaningKeys.size()) return a.remaningKeys.size() > b.remaningKeys.size();
		for (size_t i = 0; i < a.position.size(); i++)
		{
			if (a.position[i] != b.position[i]) return less(a.position[i], b.position[i]);
		}
		return false;
	}

	bool P2AStartCompWithoutDist(const P2AStarSearchNode& a, const P2AStarSearchNode& b) {
		if (a.remaningKeys.size() != b.remaningKeys.size()) return a.remaningKeys.size() < b.remaningKeys.size();
		for (size_t i = 0; i < a.position.size(); i++)
		{
			if (a.position[i] != b.position[i]) return less(a.position[i], b.position[i]);
		}
		return false;
	}


	int P2AStarFrom(P2AStarSearchNode start, std::set<Node*> keys, Map& map) {


		std::priority_queue<P2AStarSearchNode, std::vector<P2AStarSearchNode>, decltype(&P2AStarComp)> queue(&P2AStarComp);
		queue.push(start);

		std::map<P2AStarSearchNode, int, decltype(&P2AStartCompWithoutDist)> bestPath(&P2AStartCompWithoutDist);

		int setCount = INT_MAX;
		while (!queue.empty()) {
			P2AStarSearchNode node = queue.top();
			queue.pop();


			int countThis = node.remaningKeys.size();
			if (countThis < setCount) {
				setCount = countThis;
				std::cout << countThis << std::endl;
			}

			
			auto bestIt = bestPath.find(node);
			if (bestIt == bestPath.end() || bestIt->second > node.Distance()) bestPath[node] = node.Distance();
			else continue;
			

			if (node.remaningKeys.size() == 0) {
				/*for (size_t i = 0; i < node.traces.size(); i++)
				{
					std::cout << std::endl << "INDEX: " << i << std::endl;
					PrintMap(map, node.traces[i]);
				}*/
				return node.Distance();
			}

			for (int i = 0; i < node.position.size(); i++)
			{
				/*
				BFSSearchNode tmp(node.position[i], node.open_gates);
				auto bestIt = bestPaths[i].find(tmp);
				if (bestIt == bestPaths[i].end() || bestIt->second > node.distance[i]) bestPaths[i][tmp] = node.distance[i];
				else continue;*/

				for (auto& n : node.position[i]->neighbours) {
					if (n.first->type == NodeType::Gate && node.remaningKeys.count(n.first->letter) > 0) continue;
					queue.push(P2AStarSearchNode(node, n.first, i));
				}
			}
		}

		return -1;
	}

	int Day18::Part2() {
		std::string fileName = "Part18Input.txt";
		std::string input = helpers::ReadFile(fileName);
		auto rows = helpers::SplitBy(input, "\n");

		Map map;

		std::pair<int, int> startingPosition;

		std::unordered_set<Node*> gates;
		NodeSet keys;
		Node* start = nullptr;

		int rowIndex = 0;
		int column = 0;
		for (auto& row : rows) {
			std::vector<Node*> v;
			for (char c : row) {
				if (c == '@') {
					auto n = new Node(NodeType::Path);
					start = n;
					v.push_back(n);
					startingPosition = std::make_pair(rowIndex, column);
				}
				else if ('a' <= c && c <= 'z') {
					Node* n = new Node(NodeType::Key);
					n->letter = std::toupper(c);
					keys.insert(n);
					v.push_back(n);
				}
				else if ('A' <= c && c <= 'Z') {
					Node* n = new Node(NodeType::Gate);
					n->letter = c;
					gates.insert(n);
					v.push_back(n);
				}
				else if (c == '#') {
					v.push_back(new Node(NodeType::Wall));
				}
				else if (c == '.') {
					v.push_back(new Node(NodeType::Path));
				}
				v.back()->position = std::make_pair(rowIndex, column);
				column++;
			}
			map.push_back(v);
			rowIndex++;
			column = 0;
		}

		std::array<Node*, 4> starts = {
			map[startingPosition.first - 1][startingPosition.second - 1],
			map[startingPosition.first + 1][startingPosition.second + 1],
			map[startingPosition.first + 1][startingPosition.second - 1],
			map[startingPosition.first - 1][startingPosition.second + 1],
		};
		map[startingPosition.first][startingPosition.second]->type = NodeType::Wall;
		map[startingPosition.first + 1][startingPosition.second]->type = NodeType::Wall;
		map[startingPosition.first - 1][startingPosition.second]->type = NodeType::Wall;
		map[startingPosition.first][startingPosition.second + 1]->type = NodeType::Wall;
		map[startingPosition.first][startingPosition.second - 1]->type = NodeType::Wall;

		ConstructGraph(map);
		//PruneGraph(map);
		NodeSet startSet;
		for (auto n : starts) {
			startSet.insert(n);
		}

		//RemoveNonEssentialNodes(startSet, gates, keys);

		std::unordered_set<char> closed_gates;
		for (auto gate : gates) closed_gates.insert(gate->letter);


		if (start == nullptr) return -1;

		std::unordered_set<char> empty;

		int accum = 0;
		for (auto start : starts) {
			NodeSet reachable_keys;
			int open_gates = 0;
			for (auto key : keys) {
				int dist = Distance(start, key, empty);
				if (dist >= 0) {
					reachable_keys.insert(key);
				}
				else {
					open_gates |= 1 << (key->letter - 'A');
				}
			}

			accum += BFSFrom(BFSSearchNode(start, open_gates), keys);


		}


		return accum;

	}

}

