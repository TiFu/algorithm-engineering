#pragma once

#include "data_structure/graph.h"

#include <set>

typedef std::set<NodeID> partition_t;
typedef std::vector<partition_t> configuration_t;
typedef std::vector<configuration_t> population_t;

namespace graph_colouring {
    void hybridColoringAlgorithm(const graph_access &G, const uint32_t k);
}