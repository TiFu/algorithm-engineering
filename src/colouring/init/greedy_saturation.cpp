#include "colouring/init/greedy_saturation.h"

inline std::set<NodeID> toSet(const graph_access &G) {
    std::set<NodeID> nodes;
    for (NodeID n = 0; n < G.number_of_nodes(); n++) {
        nodes.insert(n);
    }
    return nodes;
}

static size_t numberOfAllowedClasses(const graph_access &G,
                                     const graph_colouring::configuration_t &s,
                                     const NodeID nodeID,
                                     const size_t k) {
    size_t count = 0;
    for (Color c = 0; c < k; c++) {
        count += static_cast<size_t>(graph_colouring::allowedInClass(G, s, c, nodeID));
    }
    return count;
}

static NodeID nextNodeWithMinAllowedClasses(const graph_access &G,
                                            const graph_colouring::configuration_t &c,
                                            const std::set<NodeID> &nodes,
                                            const size_t k) {
    assert(!nodes.empty());
    NodeID targetNode = *nodes.begin();
    size_t minNumberOfAllowedClasses = k;
    for (auto n : nodes) {
        auto count = numberOfAllowedClasses(G, c, n, k);
        if (count < minNumberOfAllowedClasses) {
            minNumberOfAllowedClasses = count;
            targetNode = n;
        }
    }
    return targetNode;
}

graph_colouring::configuration_t graph_colouring::initByGreedySaturation(const graph_access &G,
                                                                         const size_t k) {
    configuration_t s(G.number_of_nodes(), std::numeric_limits<NodeID>::max());

    auto nodes = toSet(G);

    auto v = nextNodeWithMinAllowedClasses(G, s, nodes, k);
    while (!nodes.empty() && numberOfAllowedClasses(G, s, v, k) > 0) {
        for (Color c = 0; c < k; c++) {
            if (allowedInClass(G, s, c, v)) {
                s[v] = c;
                break;
            }
        }
        nodes.erase(v);
        if (nodes.empty()) {
            break;
        }
        v = nextNodeWithMinAllowedClasses(G, s, nodes, k);
    }
    std::uniform_int_distribution<Color> distribution(0, static_cast<Color>(k - 1));
    std::mt19937 generator;
    for (NodeID n : nodes) {
        s[n] = distribution(generator);
    }
    return s;
}


