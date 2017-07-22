#include "colouring/init/greedy_saturation.h"

#include "util/graph_util.h"

static NodeID nextNodeWithMinAllowedClasses(const graph_access &G,
                                            const configuration_t &c,
                                            const std::set<NodeID> &nodes) {
    assert(!nodes.empty());
    NodeID targetNode = *nodes.begin();
    size_t minNumberOfAllowedClasses = c.size();
    for (auto n : nodes) {
        auto count = graph_colouring::numberOfAllowedClasses(G, c, n);
        if (count < minNumberOfAllowedClasses) {
            minNumberOfAllowedClasses = count;
            targetNode = n;
        }
    }
    return targetNode;
}

configuration_t graph_colouring::initByGreedySaturation(const graph_access &G,
                                                        const size_t k,
                                                        std::mt19937 &generator) {
    configuration_t s;
    s.resize(k);

    auto nodes = graph_util::toSet(G);

    auto v = nextNodeWithMinAllowedClasses(G, s, nodes);
    while (!nodes.empty() && graph_colouring::numberOfAllowedClasses(G, s, v) > 0) {
        for (auto &p : s) {
            if (graph_colouring::allowedInClass(G, p, v)) {
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


