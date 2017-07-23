#include "colouring/graph_colouring.h"

#include <algorithm>

typedef std::vector<configuration_t> population_t;

namespace graph_colouring {

    bool allowedInClass(const graph_access &G, const partition_t &p, const NodeID nodeID) {
        assert(p.count(nodeID) <= 0);
        for (auto neighbour : G.neighbours(nodeID)) {
            if (p.count(neighbour) > 0) {
                return false;
            }
        }
        return true;
    }

    size_t
    numberOfAllowedClasses(const graph_access &G, const configuration_t &c, const NodeID nodeID) {
        size_t count = 0;
        for (auto &p : c) {
            count += allowedInClass(G, p, nodeID);
        }
        return count;
    }

    bool isConflicting(const NodeID node, const partition_t &p, const graph_access &G) {
        for (auto neighbour : G.neighbours(node)) {
            if (p.count(neighbour) > 0) {
                return true;
            }
        }
        return false;
    }

    configuration_t clone(const configuration_t &s) {
        configuration_t s_copy;
        s_copy.resize(s.size());
        for (size_t i = 0; i < s.size(); i++) {
            s_copy[i].insert(s[i].begin(), s[i].end());
        }
        return s_copy;
    }

    size_t numberOfConflictingNodes(const graph_access &G,
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

    size_t numberOfConflictingEdges(const graph_access &G,
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


    static population_t initPopulation(const InitOperator &initOperator,
                                       const graph_access &G,
                                       const size_t population_size,
                                       const size_t k,
                                       std::mt19937 generator) {
        population_t P;
        P.resize(population_size);

        for (size_t i = 0; i < population_size; i++) {
            P[i] = initOperator(G, k, generator);
        }
        return P;
    }

    configuration_t parallelColoringAlgorithm(const InitOperator &initOperator,
                                              const CrossoverOperator &crossoverOperator,
                                              const LSOperator &lsOperator,
                                              const graph_access &G,
                                              const size_t k,
                                              const size_t population_size,
                                              const size_t maxItr) {
        std::mt19937 generator;

        population_t P = initPopulation(initOperator, G, population_size, k, generator);

        for (size_t itr = 0; itr < maxItr; itr++) {
            const size_t mating_population_size = population_size / 2;
            std::uniform_int_distribution<size_t> distribution(0, population_size - 1);
            for (size_t i = 0; i < mating_population_size; i++) {
                std::array<configuration_t *, 2> parents = {&P[distribution(generator)], &P[distribution(generator)]};
                auto s = lsOperator(G,
                                    crossoverOperator(G, *parents[0], *parents[1], generator),
                                    generator);
                if (graph_colouring::numberOfConflictingEdges(G, *parents[0]) >
                    graph_colouring::numberOfConflictingEdges(G, *parents[1])) {
                    *parents[0] = s;
                } else {
                    *parents[1] = s;
                }
            }
        }
        return *std::max_element(P.begin(),
                                 P.end(),
                                 [&G](const configuration_t &a, const configuration_t &b) {
                                     return graph_colouring::numberOfConflictingEdges(G, a) >
                                            graph_colouring::numberOfConflictingEdges(G, b);
                                 });
    }

}