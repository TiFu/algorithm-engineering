#pragma once

#include "data_structure/graph.h"

#include <functional>
#include <random>
#include <set>

typedef std::set<NodeID> partition_t;
typedef std::vector<partition_t> configuration_t;

namespace graph_colouring {

    typedef std::function<configuration_t(const graph_access &G,
                                          const size_t k,
                                          std::mt19937 &generator)> InitOperator;

    typedef std::function<configuration_t(const graph_access &G,
                                          const configuration_t &s1,
                                          const configuration_t &s2,
                                          std::mt19937 &generator)> CrossoverOperator;

    typedef std::function<configuration_t(const graph_access &G,
                                          const configuration_t &s,
                                          std::mt19937 &generator)> LSOperator;

    configuration_t parallelColoringAlgorithm(const InitOperator &initOperator,
                                              const CrossoverOperator &crossoverOperator,
                                              const LSOperator &lsOperator,
                                              const graph_access &G,
                                              size_t k,
                                              size_t population_size,
                                              size_t maxItr);

    bool allowedInClass(const graph_access &G, const partition_t &p, NodeID nodeID);

    size_t numberOfAllowedClasses(const graph_access &G, const configuration_t &c, NodeID nodeID);

    bool isConflicting(NodeID node, const partition_t &p, const graph_access &G);

    size_t numberOfConflictingNodes(const graph_access &G,
                                    const configuration_t &s);

    size_t numberOfConflictingEdges(const graph_access &G,
                                    const configuration_t &s);

    configuration_t clone(const configuration_t &s);
}