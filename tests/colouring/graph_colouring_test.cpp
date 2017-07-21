#include "data_structure/graph.h"
#include "data_structure/io/graph_io.h"
#include "colouring/graph_colouring.h"


#include "util/graph_util.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

template<typename T>
std::ostream &operator<<(std::ostream &strm, const std::set<T> &set) {
    strm << "{";
    bool first = true;
    for (const T &elem : set) {
        if (first) {
            first = false;
        } else {
            strm << ",";
        }
        strm << elem;
    }
    return strm << "}";
}

template<typename T>
std::ostream &operator<<(std::ostream &strm, const std::vector<T> &set) {
    strm << "[";
    bool first = true;
    for (const T &elem : set) {
        if (first) {
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
    for (auto neighbour : G.neighbours(nodeID)) {
        if (p.count(neighbour) > 0) {
            return false;
        }
    }
    return true;
}

static size_t numberOfAllowedClasses(const graph_access &G, const configuration_t &c, const NodeID nodeID) {
    size_t count = 0;
    for (auto &p : c) {
        count += allowedInClass(G, p, nodeID);
    }
    return count;
}

static inline bool isConflicting(const NodeID node, const partition_t &p, const graph_access &G) {
    for (auto neighbour : G.neighbours(node)) {
        if (p.count(neighbour) > 0) {
            return true;
        }
    }
    return false;
}

static size_t numberOfConflictingNodes(const graph_access &G,
                                       const configuration_t &s) {
    size_t count = 0;
    for (auto &p : s) {
        for (auto &n : p) {
            for (auto neighbour : G.neighbours(n)) {
                if (p.count(neighbour) > 0) {
                    count++;
                    break;
                }
            }
        }
    }
    return count;
}

static size_t numberOfConflictingEdges(const graph_access &G,
                                       const configuration_t &s) {
    size_t count = 0;
    for (auto &p : s) {
        for (auto &n : p) {
            for (auto neighbour : G.neighbours(n)) {
                count += (p.count(neighbour) > 0);
            }
        }
    }
    return count / 2;
}

std::set<NodeID> toSet(const graph_access &G) {
    std::set<NodeID> nodes;
    for (NodeID n = 0; n < G.number_of_nodes(); n++) {
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
    for (auto n : nodes) {
        auto count = numberOfAllowedClasses(G, c, n);
        if (count < minNumberOfAllowedClasses) {
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
        for (auto &p : s) {
            if (allowedInClass(G, p, v)) {
                p.insert(v);
                break;
            }
        }
        nodes.erase(v);
        if (nodes.empty()) {
            break;
        }
        v = nextNodeWithMinAllowedClasses(G, s, nodes);
    }
    std::uniform_int_distribution<size_t> distribution(0, s.size() - 1);
    for (NodeID n : nodes) {
        s[distribution(generator)].insert(n);
    }
    return s;
}

static population_t initPopulation(const graph_access &G, const size_t population_size, const uint32_t k) {
    population_t P;
    P.resize(population_size);
    std::mt19937 generator;

    for (size_t i = 0; i < population_size; i++) {
        P[i] = generatePopulation(G, k, generator);
    }
    return P;
}

static configuration_t clone(const configuration_t &s) {
    configuration_t s_copy;
    s_copy.resize(s.size());
    for (size_t i = 0; i < s.size(); i++) {
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

    configuration_t *V[2];
    V[0] = &s1;
    V[1] = &s2;

    for (size_t l = 0; l < s1.size(); l++) {
        auto A = (l & 1);
        auto v = std::max_element(V[A]->begin(),
                                  V[A]->end(),
                                  [](const partition_t &a, const partition_t &b) {
                                      return a.size() < b.size();
                                  });
        s[l].insert(v->begin(), v->end());
        v->clear();
        for (auto n : s[l]) {
            for (auto &p : *V[1 - A]) {
                p.erase(n);
            }
        }
    }
    partition_t rest;
    for (auto &p : *V[0]) {
        rest.insert(p.begin(), p.end());
    }
    std::uniform_int_distribution<size_t> distribution(0, s.size() - 1);
    s[distribution(generator)].insert(rest.begin(), rest.end());

    return s;
}

static configuration_t tabuSearchOperator(const graph_access &G,
                                          const configuration_t &s,
                                          const size_t L,
                                          const size_t A,
                                          const size_t alpha,
                                          std::mt19937 &generator) {
    configuration_t s_mutated = clone(s);
    std::uniform_int_distribution<size_t> distribution(0, A - 1);
    //tabu tenure
    const size_t tl = distribution(generator) + alpha * numberOfConflictingNodes(G, s_mutated);
    //Number of classes
    const size_t k = s_mutated.size();
    //Number of nodes
    const size_t V = G.number_of_nodes();

    std::vector<size_t> tabu_table(V * k);
    for (size_t l = 0; l < L; l++) {
        //std::cerr << "l: " << l << "\n";
        NodeID best_v = std::numeric_limits<NodeID>::max();
        size_t best_i = std::numeric_limits<size_t>::max();
        size_t best_c_v = std::numeric_limits<size_t>::max();
        size_t best_score = numberOfConflictingEdges(G, s_mutated);
        for (size_t c_v = 0; c_v < k; c_v++) {
            for (auto v : s_mutated[c_v]) {
                if (isConflicting(v, s_mutated[c_v], G)) {
                    for (size_t i = 0; i < k; i++) {
                        if (i == c_v || tabu_table[v * k + c_v] > l) {
                            continue;
                        }
                        auto s_new = clone(s_mutated);
                        s_new[c_v].erase(v);
                        s_new[i].insert(v);
                        auto new_score = numberOfConflictingEdges(G, s_new);
                        if (new_score <= best_score) {
                            best_v = v;
                            best_i = i;
                            best_c_v = c_v;
                            best_score = new_score;
                        }
                    }
                }
            }
        }
        //nothing to improve
        if (best_v == std::numeric_limits<NodeID>::max()) {
            break;
        }
        tabu_table[best_v * k + best_c_v] = ((l + 1) + tl);
        s_mutated[best_c_v].erase(best_v);
        s_mutated[best_i].insert(best_v);

        //std::cerr << "(v,i) = (" << best_v << "," << best_i << ") " << s_mutated << "\n";
        //std::cerr << tabu_table << "\n";
    }

    return s_mutated;
}

static void algorithm(const graph_access &G, const uint32_t k) {
    const uint32_t population_size = 10;

    //std::cerr << graph_util::toGraphvizStrig(G);

    population_t P = initPopulation(G, population_size, k);
}


TEST(graph_colouring, Testtest) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    //std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    algorithm(G, 2);
    //EXPECT_EQ(2, 3);
}

TEST(graph_colouring, numberOfConflictingEdges) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    std::mt19937 generator;

    configuration_t s_zero_score = generatePopulation(G, 3, generator);
    EXPECT_EQ(numberOfConflictingEdges(G, s_zero_score), 0);

    configuration_t s_two_classes_score;
    s_two_classes_score.resize(3);
    s_two_classes_score[0].insert(0);
    s_two_classes_score[0].insert(1);
    s_two_classes_score[0].insert(2);
    s_two_classes_score[0].insert(3);
    s_two_classes_score[0].insert(4);
    s_two_classes_score[1].insert(5);
    EXPECT_EQ(numberOfConflictingEdges(G, s_two_classes_score), 4);

    configuration_t s_worst_score;
    s_worst_score.resize(3);
    for (NodeID n = 0; n < G.number_of_nodes(); ++n) {
        s_worst_score[0].insert(n);
    }
    EXPECT_EQ(numberOfConflictingEdges(G, s_worst_score), 7);
}

TEST(graph_colouring, numberOfConflictingNodes) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    std::mt19937 generator;

    configuration_t s_zero_score = generatePopulation(G, 3, generator);
    EXPECT_EQ(numberOfConflictingNodes(G, s_zero_score), 0);

    configuration_t s_two_classes_score;
    s_two_classes_score.resize(3);
    s_two_classes_score[0].insert(0);
    s_two_classes_score[0].insert(1);
    s_two_classes_score[0].insert(2);
    s_two_classes_score[0].insert(3);
    s_two_classes_score[0].insert(4);
    s_two_classes_score[1].insert(5);
    EXPECT_EQ(numberOfConflictingNodes(G, s_two_classes_score), 5);

    configuration_t s_worst_score;
    s_worst_score.resize(3);
    for (NodeID n = 0; n < G.number_of_nodes(); ++n) {
        s_worst_score[0].insert(n);
    }
    EXPECT_EQ(numberOfConflictingNodes(G, s_worst_score), 6);
}

TEST(graph_colouring, gpxCrossover) {
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

    //Example from "Hybrid Evolutionary Algorithms for Graph Coloring"
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

TEST(graph_colouring, tabuSearchOperator) {
    std::mt19937 generator;

    graph_access G;
    graph_io::readGraphWeighted(G, "../../input/simple.graph");
    configuration_t s_small_graph;
    s_small_graph.resize(3);
    s_small_graph[0].insert(0);
    s_small_graph[0].insert(1);
    s_small_graph[0].insert(2);
    s_small_graph[1].insert(3);
    s_small_graph[1].insert(4);
    s_small_graph[1].insert(5);

    auto s_small_graph_opt = tabuSearchOperator(G, s_small_graph, 10, 3, 2, generator);
    ASSERT_THAT(s_small_graph_opt[0], testing::ElementsAre(0, 2));
    ASSERT_THAT(s_small_graph_opt[1], testing::ElementsAre(3, 5));
    ASSERT_THAT(s_small_graph_opt[2], testing::ElementsAre(1, 4));
}

TEST(graph_colouring, tabuSearchOperator_miles250_sorted) {
    std::mt19937 generator;

    graph_access G;
    graph_io::readGraphWeighted(G, "../../input/miles250-sorted.graph");
    configuration_t s_init = generatePopulation(G, 5, generator);
    ASSERT_EQ(numberOfConflictingEdges(G, s_init), 71);

    auto s_opt = tabuSearchOperator(G, s_init, 100, 3, 2, generator);
    ASSERT_EQ(numberOfConflictingEdges(G, s_opt), 16);
}
