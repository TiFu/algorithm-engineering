#include "util/graph_util.h"

#include <sstream>

std::string graph_util::toGraphvizStrig(const graph_access &G, const std::string label) {
    std::stringstream ss;
    ss << "graph " << label << " {\n";
    std::vector<bool> covered_edges(G.number_of_nodes()* G.number_of_nodes(), false);
    for(NodeID n = 0; n < G.number_of_nodes(); ++n) {
        for (auto neighbour : G.neighbours(n)) {
            if(!covered_edges[n*G.number_of_nodes() + neighbour]) {
                ss << "    " << n << " -- " << neighbour << ";\n";
                covered_edges[n*G.number_of_nodes() + neighbour] = true;
                covered_edges[neighbour*G.number_of_nodes() + n] = true;
            }
        }
    }
    ss << "}\n";
    return ss.str();
}