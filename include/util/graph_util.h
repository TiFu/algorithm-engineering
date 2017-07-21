#pragma once

#include "colouring/graph_colouring.h"

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

namespace graph_util {
    inline std::set<NodeID> toSet(const graph_access &G) {
        std::set<NodeID> nodes;
        for (NodeID n = 0; n < G.number_of_nodes(); n++) {
            nodes.insert(n);
        }
        return nodes;
    }
    std::string toGraphvizStrig(const graph_access &G,
                                const std::string &label = "G");
    std::string toGraphvizStrig(const graph_access &G,
                                const configuration_t &configuration,
                                const bool partitioned = false,
                                const std::string &label = "G");
}
