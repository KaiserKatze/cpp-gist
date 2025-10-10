#pragma once

// 给定哈斯图（本质上是一种有向无环图），计算对应的偏序格、代数格中各个元素的补元

#include <string>
#include <vector>
#include <algorithm>
#include <initializer_list>
#include <utility>
#include "Graph.hpp"

struct Node {
    size_t id;
    std::string name;
};

using NodeRef = std::reference_wrapper<const Node>;

static constexpr size_t INVALID = std::numeric_limits<size_t>::max();


struct Lattice {
    std::vector<Node> nodes;  // 记录各个结点的代数符号
    OrthogonalListGraph<size_t, size_t> graph;  // 用十字链表法表示的哈斯图

    Lattice(std::initializer_list<Node>&& nodes,
        std::initializer_list<std::pair<size_t, size_t>>&& edges)
        : nodes{ nodes }, graph(nodes.size()) {
        // 默认输入是有向无环图
        for (auto&& [first, second] : edges) {  // 逐个连接有向弧
            graph.Connect(first, second, 1);
        }
    }

    bool less_equal(const size_t& first, const size_t& second) const {
        // 计算两个结点的序关系
        // 如果 first 是 second 的前趋，返回 true；否则返回 false
        if (!(0 <= first && first < nodes.size()
            && 0 <= second && second < nodes.size())) {
            throw std::out_of_range{ "Invalid node id" };
        }
        if (first == second) {
#ifdef DEBUG
            std::clog << "Node[" << first << "](" << nodes[first].name << ") is identical!\n";
#endif
            return true;
        }
        std::vector<size_t> to_visit{ first };
        while (!to_visit.empty()) {
            const size_t current{ to_visit.back() };
            to_visit.pop_back();
            const auto& currentNode{ graph.vertices[current] };
            const auto* nextArc{ currentNode.firstout };
            while (nextArc) {
                const size_t& headvex{ nextArc->headvex };
                if (headvex == second) {
#ifdef DEBUG
                    std::clog << "Node[" << first << "](" << nodes[first].name << ") <= "
                        << "Node[" << second << "](" << nodes[second].name << ")\n";
#endif
                    return true;
                }
                else {
                    to_visit.push_back(headvex);
                }
                nextArc = nextArc->tlink;
            }
        }
#ifdef DEBUG
        std::clog << "Node[" << first << "](" << nodes[first].name << ") !<= "
            << "Node[" << second << "](" << nodes[second].name << ")\n";
#endif
        return false;
    }

    std::vector<size_t> upper_bound(const std::vector<size_t>& vexset) const {
        // 计算上界
        std::vector<size_t> result;
        for (size_t i{ 0 }; i < nodes.size(); ++i) {
            bool ge{ true };
            for (const size_t& vex : vexset) {
                if (!less_equal(vex, i)) {
                    ge = false;
                    break;
                }
            }
            if (ge) {
                result.push_back(i);
            }
        }
#ifdef DEBUG
        std::clog << "Upper bound of { ";
        for (const size_t& vex : vexset) {
            std::clog << nodes[vex].name << ", ";
        }
        std::clog << "} is { ";
        for (const size_t& vex : result) {
            std::clog << nodes[vex].name << ", ";
        }
        std::clog << "}\n";
#endif
        return result;
    }

    std::vector<size_t> lower_bound(const std::vector<size_t>& vexset) const {
        // 计算下界
        std::vector<size_t> result;
        for (size_t i{ 0 }; i < nodes.size(); ++i) {
            bool le{ true };
            for (const size_t& vex : vexset) {
                if (!less_equal(i, vex)) {
                    le = false;
                    break;
                }
            }
            if (le) {
                result.push_back(i);
            }
        }
#ifdef DEBUG
        std::clog << "Lower bound of { ";
        for (const size_t& vex : vexset) {
            std::clog << nodes[vex].name << ", ";
        }
        std::clog << "} is { ";
        for (const size_t& vex : result) {
            std::clog << nodes[vex].name << ", ";
        }
        std::clog << "}\n";
#endif
        return result;
    }

    size_t join(const std::vector<size_t>& vexset) const {
        // 给定结点 id 集合，尝试计算它们的最小上界
        // 如果最小上界不存在，返回 INVALID；
        // 如果最小上界存在，返回最小上界对应的结点的 id
        const auto bound{ upper_bound(vexset) };
        for (const size_t& v1 : bound) {
            bool le{ true };
            for (const size_t& v2 : bound) {
                if (!less_equal(v1, v2)) {
                    le = false;
                    break;
                }
            }
            if (le) {
                return v1;
            }
        }
        return INVALID;
    }

    size_t meet(const std::vector<size_t>& vexset) const {
        // 给定结点 id 集合，尝试计算它们的最大下界
        // 如果最大下界不存在，返回 INVALID；
        // 如果最大下界存在，返回最大下界对应的结点的 id
        const auto bound{ lower_bound(vexset) };
        for (const size_t& v1 : bound) {
            bool ge{ true };
            for (const size_t& v2 : bound) {
                if (!less_equal(v2, v1)) {
                    ge = false;
                    break;
                }
            }
            if (ge) {
                return v1;
            }
        }
        return INVALID;
    }

    bool isValidLattice() const {
        // 检查用户输入是否成为偏序格
        // 对于偏序集，只有当集合中任意两个元素都存在最小上界和最大下界时，才能成为偏序格
        for (size_t i{ 0 }; i < nodes.size(); ++i) {
            for (size_t j{ 0 }; j < nodes.size(); ++j) {
                if (i == j) {
                    continue;
                }
                const size_t sup{ join({i,j}) };
                const size_t inf{ meet({i,j}) };
                if (sup == INVALID || inf == INVALID) {
                    return false;
                }
            }
        }
        return true;
    }

};

void TestLattice() {
    // 钻石格
    Lattice lattice1{
    {
        {0, "0"},
        {1, "1"},
        {2, "a"},
        {3, "b"},
        {4, "c"},
    },
    {
        {0, 2},
        {0, 3},
        {0, 4},
        {2, 1},
        {3, 1},
        {4, 1},
    }
    };

    assert(lattice1.less_equal(0, 0));
    assert(lattice1.less_equal(0, 1));
    assert(lattice1.less_equal(0, 2));
    assert(lattice1.less_equal(0, 3));
    assert(lattice1.less_equal(0, 4));
    assert(lattice1.less_equal(2, 1));
    assert(lattice1.less_equal(3, 1));
    assert(lattice1.less_equal(4, 1));

    lattice1.lower_bound({ 0, 2 });
    lattice1.upper_bound({ 0, 2 });

    assert(lattice1.join({ 3, 4 }) == 1);
    assert(lattice1.meet({ 2, 1 }) == 2);
    assert(lattice1.meet({ 2, 3 }) == 0);
    assert(lattice1.meet({ 2, 4 }) == 0);
    assert(lattice1.join({ 0, 0 }) == 0);
    assert(lattice1.meet({ 3, 4 }) == 0);
    assert(lattice1.join({ 2, 0 }) == 2);
    assert(lattice1.join({ 2, 3 }) == 1);
    assert(lattice1.join({ 2, 4 }) == 1);
    assert(lattice1.meet({ 1, 1 }) == 1);

    std::cout << "Lattice validation: "
        << std::boolalpha << lattice1.isValidLattice()
        << '\n';

    // 非偏序格
    Lattice lattice2{
    {
        {0, "0"},
        {1, "1"},
        {2, "a"},
        {3, "b"},
        {4, "c"},
        {5, "d"},
    },
    {
        {0, 3},
        {0, 4},
        {3, 2},
        {3, 5},
        {4, 2},
        {4, 5},
        {2, 1},
        {5, 1},
    }
    };

    std::cout << "Lattice validation: "
        << std::boolalpha << lattice2.isValidLattice()
        << '\n';
}
