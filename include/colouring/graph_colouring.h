#pragma once

#include "data_structure/graph.h"

#include <functional>
#include <random>
#include <set>

namespace graph_colouring {

    typedef std::vector<Color> Configuration;

    typedef std::function<Configuration(const graph_access &G,
                                        const size_t k)> InitOperator;

    typedef std::function<Configuration(const graph_access &G,
                                        const Configuration &s1,
                                        const Configuration &s2)> CrossoverOperator;

    typedef std::function<Configuration(const graph_access &G,
                                        const Configuration &s)> LSOperator;

    typedef std::function<bool(const graph_access &G,
                               const Configuration &a,
                               const Configuration &b)> ConfugirationCompare;

    Configuration coloringAlgorithm(const InitOperator &initOperator,
                                    const CrossoverOperator &crossoverOperator,
                                    const LSOperator &lsOperator,
                                    const ConfugirationCompare &cmp,
                                    const graph_access &G,
                                    size_t k,
                                    size_t population_size,
                                    size_t maxItr);

    Configuration parallelColoringAlgorithm(const InitOperator &initOperator,
                                            const CrossoverOperator &crossoverOperator,
                                            const LSOperator &lsOperator,
                                            const ConfugirationCompare &cmp,
                                            const graph_access &G,
                                            size_t k,
                                            size_t population_size,
                                            size_t maxItr);

    size_t colorCount(const Configuration &s);

    bool allowedInClass(const graph_access &G,
                        const Configuration &c,
                        Color color,
                        NodeID nodeID);

    size_t numberOfConflictingNodes(const graph_access &G,
                                    const Configuration &s);

    size_t numberOfConflictingEdges(const graph_access &G,
                                    const Configuration &s);

}