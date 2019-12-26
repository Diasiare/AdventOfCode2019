#include "Day20.h"

#include <vector>
#include <map>
#include <unordered_set>
#include <queue>
#include <set>
#include <functional>
#include <utility>
#include <string>
#include <iostream>
#include "helpers.h"

namespace day20 {
	class Node;
	typedef std::vector<std::vector<Node*>> Map;
	typedef std::set<Node*> NodeSet;

	int nextId = 0;

	enum class NodeType {
		Portal,
		Wall,
		Path,
	};

	class Node {
	public:
		std::map<Node*, int> neighbours;
		std::pair<int, int> position;
		NodeType type;
		std::string letter;
		bool isOuter;

		Node(NodeType type) : type(type), letter("") {}

		void AddNeighbour(Node* other, int distance) {
			neighbours[other] = distance;
			other->neighbours[this] = distance;
		}

		void Cleanup() {
			// Do not clean up important nodes
			if (type == NodeType::Portal) return;
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

	void ConstructGraph(Map& map) {
		std::vector<Node*> portals;
		
		for (size_t i = 0; i < map.size(); i++)
		{
			for (int i2 = 0; i2 < map[i].size(); i2++) {
				auto c = map[i][i2];
				if (c->type == NodeType::Wall) continue;
				MaybeAdd(c, map[i - 1][i2]);
				MaybeAdd(c, map[i][i2 + 1]);
				MaybeAdd(c, map[i + 1][i2]);
				MaybeAdd(c, map[i][i2 - 1]);
				if (c->type == NodeType::Portal) {
					portals.push_back(c);
				}
			}
		}

		int outerLeftCol = map[0].size() - 3;
		int outerBottomRow = map.size() - 3;
		for (auto p1 : portals) {
			auto& pos = p1->position;
			if (pos.first == 2 || pos.first == outerBottomRow || pos.second == 2 || pos.second == outerLeftCol) {
				p1->isOuter = true;
			}
			else {
				p1->isOuter = false;
			}

			for (auto p2 : portals) {
				if (p1->letter == p2->letter && p1 != p2) {
					p1->AddNeighbour(p2, 1);
				}
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

	auto less = std::less<Node *>();

	int Distance(Node* start, Node* end) {
		std::set<Node*> visited;
		typedef std::pair<int, Node*> DNode;

		auto comp = [](const DNode& a, const DNode& b) {
			if (a.first == b.first) return less( a.second, b.second);
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


			for (auto next : n->neighbours) {
				if (visited.count(next.first) > 0) continue;
				search.push(std::make_pair(current_distance + next.second, next.first));
			}
		}
		return -1;
	}

	Map BuildMap() {
		std::string fileName = "Part20Input.txt";
		std::string input = helpers::ReadFile(fileName);
		auto rows = helpers::SplitBy(input, "\n");

		Map map;

		int rowIndex = 0;
		int column = 0;
		for (auto& row : rows) {
			std::vector<Node*> v;
			for (char c : row) {
				if (c == '.') {
					v.push_back(new Node(NodeType::Path));
				}
				else {
					v.push_back(new Node(NodeType::Wall));
				}
				v.back()->position = std::make_pair(rowIndex, column);
				column++;
			}
			map.push_back(v);
			rowIndex++;
			column = 0;
		}

		for (auto row : map) {
			for (auto node : row) {
				if (node->type != NodeType::Path) continue;
				char c;
				c = rows[node->position.first + 1][node->position.second];
				if ('A' <= c && c <= 'Z') {
					node->letter = std::string({ c, rows[node->position.first + 2][node->position.second] });
					node->type = NodeType::Portal;
				}
				c = rows[node->position.first - 1][node->position.second];
				if ('A' <= c && c <= 'Z') {
					node->letter = std::string({ rows[node->position.first - 2][node->position.second], c });
					node->type = NodeType::Portal;
				}
				c = rows[node->position.first][node->position.second + 1];
				if ('A' <= c && c <= 'Z') {
					node->letter = std::string({ c, rows[node->position.first][node->position.second + 2] });
					node->type = NodeType::Portal;
				}
				c = rows[node->position.first][node->position.second - 1];
				if ('A' <= c && c <= 'Z') {
					node->letter = std::string({rows[node->position.first][node->position.second - 2], c });
					node->type = NodeType::Portal;
				}
			}
		}
		return map;
	}


	int P2Distance(Node* start, Node* end) {
		std::set<std::pair<Node *, int>> visited;
		class DNode {
		public:
			int distance;
			Node* node;
			int layer;
			DNode * prev;

			DNode(Node* node) : node(node), distance(0), layer(0) {};
			DNode(DNode& old_node, Node* new_pos) {
				Node* old_pos = old_node.node;
				distance = old_node.distance + old_pos->neighbours[new_pos];
				node = new_pos;
				prev = new DNode(old_node);
				layer = old_node.layer;
				if (new_pos->type == NodeType::Portal && old_pos->type == NodeType::Portal && new_pos->letter == old_pos->letter) {
					layer += new_pos->isOuter ? 1 : -1;
					if (layer < 0) {
						std::cout << layer << std::endl;
					}
				}
			}
		};

		auto comp = [](const DNode& a, const DNode& b) {
			if (a.distance != b.distance) b.distance <  a.distance;
			if (a.node != b.node) less(a.node, b.node);
			return a.layer > b.layer;
		};
		std::priority_queue<DNode, std::vector<DNode>, std::function<bool(const DNode&, const DNode&)>> search(comp);
		search.push(DNode(start));

		while (!search.empty()) {
			auto next = search.top();
			Node* n = next.node;
			int current_distance = next.distance;
			if (n == end && next.layer == 0) {
				return current_distance;
			}
			search.pop();
			std::pair<Node*, int> p = std::make_pair(n, next.layer);
			if (visited.count(p) > 0) continue;
			visited.insert(p);


			for (auto nei : n->neighbours) {
				// We are stepping through an outer portal on layer 0
				if (nei.first->type == NodeType::Portal && n->type == NodeType::Portal && n->letter == nei.first->letter && n->isOuter && next.layer == 0) continue;
				DNode new_node(next, nei.first);
				if (visited.count(std::make_pair(new_node.node, new_node.layer)) > 0) continue;
				search.push(new_node);
			}
		}
		return -1;
	}

	int Day20::Part1() {
		Map map = BuildMap();
		ConstructGraph(map);
		PruneGraph(map);

		Node* start = nullptr;
		Node* end = nullptr;

		for (auto r : map) {
			for (auto n : r) {
				if (n->letter == "AA") start = n;
				if (n->letter == "ZZ") end = n;
			}
		}

		return Distance(start, end);

	}

	int Day20::Part2() {

		Map map = BuildMap();
		ConstructGraph(map);
		PruneGraph(map);

		Node* start = nullptr;
		Node* end = nullptr;

		for (auto r : map) {
			for (auto n : r) {
				if (n->letter == "AA") start = n;
				if (n->letter == "ZZ") end = n;
			}
		}

		return P2Distance(start, end);
	}

}