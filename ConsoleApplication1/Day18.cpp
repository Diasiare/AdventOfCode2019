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

	void RemoveNonEssentialNodes(Node* start, std::unordered_set<Node*> gates, NodeSet keys) {
		std::unordered_set<char> closed_gates;
		for (auto gate : gates) closed_gates.insert(gate->letter);
		
		std::vector<Node*> all;
		all.push_back(start);
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
				if (other->type == NodeType::Path && other != start) it = n->neighbours.erase(it);
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

				column++;
			}
			map.push_back(v);
			rowIndex++;
			column = 0;
		}

		ConstructGraph(map);
		RemoveNonEssentialNodes(start, gates, keys);

		std::unordered_set<char> closed_gates;
		for (auto gate : gates) closed_gates.insert(gate->letter);

		if (start == nullptr) return -1;

		SearchNode start_node(start, closed_gates, keys, 0);

		return SearchBy(start_node);
	}

}

