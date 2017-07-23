#pragma once

#include "data_structure/graph.h"

#include <random>
#include <set>

typedef std::set<NodeID> partition_t;
typedef std::vector<partition_t> configuration_t;

namespace graph_colouring {
    bool allowedInClass(const graph_access &G, const partition_t &p, NodeID nodeID);

    size_t numberOfAllowedClasses(const graph_access &G, const configuration_t &c, NodeID nodeID);

    bool isConflicting(NodeID node, const partition_t &p, const graph_access &G);

    size_t numberOfConflictingNodes(const graph_access &G,
                                    const configuration_t &s);

    size_t numberOfConflictingEdges(const graph_access &G,
                                    const configuration_t &s);

    configuration_t clone(const configuration_t &s);
}