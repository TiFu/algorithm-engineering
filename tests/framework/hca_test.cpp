#include "data_structure/graph.h"
#include "data_structure/io/graph_io.h"
//#include "hca/hca.h"

#include <gtest/gtest.h>

typedef std::set<NodeID> partition_t;
typedef std::vector<partition_t> partition_list_t;
typedef std::vector<partition_list_t> population_t;

template<typename T>
std::ostream& operator<<(std::ostream &strm, const std::set<T>& set) {
    strm << "{";
    bool first = true;
    for(const T &elem : set) {
        if(first) {
            first = false;
        } else {
            strm << ",";
        }
        strm << elem;
    }
    return strm << "}";
}

template<typename T>
std::ostream& operator<<(std::ostream &strm, const std::vector<T>& set) {
    strm << "[";
    bool first = true;
    for(const T &elem : set) {
        if(first) {
            first = false;
        } else {
            strm << ",";
        }
        strm << elem;
    }
    return strm << "]";
}

static bool allowedInClass(const graph_access &G, const partition_t &p, const NodeID nodeID) {
    assert(p.count(nodeID) <= 0);
    for(auto neighbour : G.neighbours(nodeID)) {
        if(p.count(neighbour) > 0) {
            return false;
        }
    }
    return true;
}

static size_t numberOfAllowedClasses(const graph_access &G, const partition_list_t &P, const NodeID nodeID) {
    size_t count = 0;
    for(auto &p : P) {
        count += allowedInClass(G, p, nodeID);
    }
    return count;
}

std::set<NodeID> toSet(const graph_access &G) {
    std::set<NodeID> nodes;
    for(NodeID n = 0; n < G.number_of_nodes(); n++) {
        nodes.insert(n);
    }
    return nodes;
}

static NodeID nextNodeWithMinAllowedClasses(const graph_access &G,
                                            const partition_list_t &P,
                                            const std::set<NodeID> &nodes) {
    assert(!nodes.empty());
    NodeID targetNode = *nodes.begin();
    size_t minNumberOfAllowedClasses = P.size();
    for(auto n : nodes) {
        auto count = numberOfAllowedClasses(G, P, n);
        if(count < minNumberOfAllowedClasses) {
            minNumberOfAllowedClasses = count;
            targetNode = n;
        }
    }
    return targetNode;
}

static partition_list_t generatePopulation(const graph_access &G, const uint32_t k, std::mt19937 &generator) {
    partition_list_t P;
    P.resize(k);

    auto nodes = toSet(G);

    auto v = nextNodeWithMinAllowedClasses(G, P, nodes);
    while (!nodes.empty() && numberOfAllowedClasses(G, P, v) > 0) {
        for(auto &p : P) {
            if(allowedInClass(G, p, v)) {
                p.insert(v);
                break;
            }
        }
        nodes.erase(v);
        if(nodes.empty()) {
            break;
        }
        v = nextNodeWithMinAllowedClasses(G, P, nodes);
    }
    std::uniform_int_distribution<size_t> distribution(0, P.size()-1);
    for(NodeID n : nodes) {
        P[distribution(generator)].insert(n);
    }

    std::cerr << P << "\n";
    return P;
}

static partition_list_t initPopulation(const graph_access &G, const size_t population_size, const uint32_t k) {
    population_t P;
    P.resize(population_size);
    std::mt19937 generator;

    for(size_t i = 0; i < population_size; i++) {
        P[i] = generatePopulation(G, k, generator);
    }
}

static void hca(const graph_access &G, const uint32_t k) {
    const uint32_t population_size = 100;

    partition_list_t P = initPopulation(G, population_size, k);
}

TEST(Test, Testtest) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";

    graph_io::readGraphWeighted(G, graph_filename);
    hca(G, 2);
    //EXPECT_EQ(2, 3);
}
