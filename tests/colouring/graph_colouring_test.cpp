#include "data_structure/graph.h"
#include "data_structure/io/graph_io.h"
#include "colouring/graph_colouring.h"

#include "util/graph_util.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

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

static size_t numberOfAllowedClasses(const graph_access &G, const configuration_t &c, const NodeID nodeID) {
    size_t count = 0;
    for(auto &p : c) {
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
                                            const configuration_t &c,
                                            const std::set<NodeID> &nodes) {
    assert(!nodes.empty());
    NodeID targetNode = *nodes.begin();
    size_t minNumberOfAllowedClasses = c.size();
    for(auto n : nodes) {
        auto count = numberOfAllowedClasses(G, c, n);
        if(count < minNumberOfAllowedClasses) {
            minNumberOfAllowedClasses = count;
            targetNode = n;
        }
    }
    return targetNode;
}

static configuration_t generatePopulation(const graph_access &G,
                                          const uint32_t k,
                                          std::mt19937 &generator) {
    configuration_t s;
    s.resize(k);

    auto nodes = toSet(G);

    auto v = nextNodeWithMinAllowedClasses(G, s, nodes);
    while (!nodes.empty() && numberOfAllowedClasses(G, s, v) > 0) {
        for(auto &p : s) {
            if(allowedInClass(G, p, v)) {
                p.insert(v);
                break;
            }
        }
        nodes.erase(v);
        if(nodes.empty()) {
            break;
        }
        v = nextNodeWithMinAllowedClasses(G, s, nodes);
    }
    std::uniform_int_distribution<size_t> distribution(0, s.size()-1);
    for(NodeID n : nodes) {
        s[distribution(generator)].insert(n);
    }
    return s;
}

static population_t initPopulation(const graph_access &G, const size_t population_size, const uint32_t k) {
    population_t P;
    P.resize(population_size);
    std::mt19937 generator;

    for(size_t i = 0; i < population_size; i++) {
        P[i] = generatePopulation(G, k, generator);
    }
    return P;
}

static configuration_t clone(const configuration_t &s) {
    configuration_t s_copy;
    s_copy.resize(s.size());
    for(size_t i = 0; i < s.size() ; i++) {
        s_copy[i].insert(s[i].begin(), s[i].end());
    }
    return s_copy;
}

static configuration_t gpxCrossover(const configuration_t &s1_org,
                                    const configuration_t &s2_org,
                                    std::mt19937 &generator) {
    assert(s1_org.size() == s2_org.size());

    auto s1 = clone(s1_org);
    auto s2 = clone(s2_org);

    configuration_t s;
    s.resize(s1.size());

    configuration_t* V[2];
    V[0] = &s1;
    V[1] = &s2;

    for(size_t l = 0; l < s1.size(); l++) {
        auto A = (l & 1);
        auto v =  std::max_element(V[A]->begin(),
                                   V[A]->end(),
                                   [](const partition_t &a, const partition_t &b) {
                                       return a.size() < b.size();
                                   });
        s[l].insert(v->begin(), v->end());
        v->clear();
        for(auto n : s[l]) {
            for(auto &p : *V[1-A]) {
                p.erase(n);
            }
        }
    }
    partition_t rest;
    for(auto &p : *V[0]) {
        rest.insert(p.begin(), p.end());
    }
    std::uniform_int_distribution<size_t> distribution(0, s.size()-1);
    s[distribution(generator)].insert(rest.begin(), rest.end());

    return s;
}

static void algorithm(const graph_access &G, const uint32_t k) {
    const uint32_t population_size = 10;

    //std::cerr << graph_util::toGraphvizStrig(G);

    population_t P = initPopulation(G, population_size, k);
}

TEST(Test, Testtest) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    //std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    algorithm(G, 2);
    //EXPECT_EQ(2, 3);
}

TEST(Test, gpxCrossover) {
    NodeID A = 0;
    NodeID B = 1;
    NodeID C = 2;
    NodeID D = 3;
    NodeID E = 4;
    NodeID F = 5;
    NodeID G = 6;
    NodeID H = 7;
    NodeID I = 8;
    NodeID J = 9;

    configuration_t s1;
    s1.resize(3);

    s1[0].insert(A);
    s1[0].insert(B);
    s1[0].insert(C);

    s1[1].insert(D);
    s1[1].insert(E);
    s1[1].insert(F);
    s1[1].insert(G);

    s1[2].insert(H);
    s1[2].insert(I);
    s1[2].insert(J);

    configuration_t s2;
    s2.resize(3);

    s2[0].insert(C);
    s2[0].insert(D);
    s2[0].insert(E);
    s2[0].insert(G);

    s2[1].insert(A);
    s2[1].insert(F);
    s2[1].insert(I);

    s2[2].insert(B);
    s2[2].insert(H);
    s2[2].insert(J);

    std::mt19937 generator;
    auto s = gpxCrossover(s1, s2, generator);
    ASSERT_EQ(s.size(), s1.size());
    ASSERT_THAT(s[0], testing::ElementsAre(D, E, F, G));
    ASSERT_THAT(s[1], testing::ElementsAre(B, H, J));
    ASSERT_THAT(s[2], testing::ElementsAre(A, C, I));
}
