#include "util/graph_util.h"

#include <sstream>
#include <iomanip>

std::string graph_util::toGraphvizStrig(const graph_access &G,
                                        const std::string &label) {
    std::stringstream ss;
    ss << "graph " << label << " {\n";
    std::vector<bool> covered_edges(G.number_of_nodes() * G.number_of_nodes(), false);
    for (NodeID n = 0; n < G.number_of_nodes(); ++n) {
        for (auto neighbour : G.neighbours(n)) {
            if (!covered_edges[n * G.number_of_nodes() + neighbour]) {
                ss << "    " << n << " -- " << neighbour << ";\n";
                covered_edges[n * G.number_of_nodes() + neighbour] = true;
                covered_edges[neighbour * G.number_of_nodes() + n] = true;
            }
        }
    }
    ss << "}\n";
    return ss.str();
}

std::string graph_util::toGraphvizStrig(const graph_access &G,
                                        const graph_colouring::configuration_t &configuration,
                                        const bool partitioned,
                                        const std::string &label) {
    std::stringstream ss;

    std::vector<std::string> colors;
    colors.resize(graph_colouring::colorCount(configuration));
    double hueFactor = 1.0 / colors.size();
    for (int i = 0; i < colors.size(); i++) {
        std::stringstream colorSS;
        colorSS << std::setprecision(3) << '"' << i * hueFactor / 1.0 << " " << 0.5 << " " << 0.5 << '"';
        colors[i] = colorSS.str();
    }

    ss << "graph " << label << " {\n";
    if (partitioned) {
        for (int i = 0; i < colors.size(); i++) {
            ss << "    subgraph cluster_" << i << " {\n";
            for (size_t n = 0; n < configuration.size(); n++) {
                if(configuration[n] == i) {
                    ss << "        " << n << " [penwidth=3 color=" << colors[configuration[n]] << "];\n";
                }
            }
            ss << "    }\n";
        }
    } else {
        for (size_t n = 0; n < configuration.size(); n++) {
                ss << "    " << n << " [penwidth=3 color=" << colors[configuration[n]] << "];\n";
        }
    }

    std::vector<bool> covered_edges(G.number_of_nodes() * G.number_of_nodes(), false);
    for (NodeID n = 0; n < G.number_of_nodes(); ++n) {
        for (auto neighbour : G.neighbours(n)) {
            if (!covered_edges[n * G.number_of_nodes() + neighbour]) {
                ss << "    " << n << " -- " << neighbour << ";\n";
                covered_edges[n * G.number_of_nodes() + neighbour] = true;
                covered_edges[neighbour * G.number_of_nodes() + n] = true;
            }
        }
    }
    ss << "}\n";
    return ss.str();
}