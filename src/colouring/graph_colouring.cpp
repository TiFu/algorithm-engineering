#include "colouring/graph_colouring.h"

bool graph_colouring::allowedInClass(const graph_access &G, const partition_t &p, const NodeID nodeID) {
    assert(p.count(nodeID) <= 0);
    for (auto neighbour : G.neighbours(nodeID)) {
        if (p.count(neighbour) > 0) {
            return false;
        }
    }
    return true;
}

size_t graph_colouring::numberOfAllowedClasses(const graph_access &G, const configuration_t &c, const NodeID nodeID) {
    size_t count = 0;
    for (auto &p : c) {
        count += allowedInClass(G, p, nodeID);
    }
    return count;
}

bool graph_colouring::isConflicting(const NodeID node, const partition_t &p, const graph_access &G) {
    for (auto neighbour : G.neighbours(node)) {
        if (p.count(neighbour) > 0) {
            return true;
        }
    }
    return false;
}

configuration_t graph_colouring::clone(const configuration_t &s) {
    configuration_t s_copy;
    s_copy.resize(s.size());
    for (size_t i = 0; i < s.size(); i++) {
        s_copy[i].insert(s[i].begin(), s[i].end());
    }
    return s_copy;
}

size_t graph_colouring::numberOfConflictingNodes(const graph_access &G,
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

size_t graph_colouring::numberOfConflictingEdges(const graph_access &G,
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

