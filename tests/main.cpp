#include <data_structure/graph.h>
#include <data_structure/io/graph_io.h>

#include <iostream>

int main(int argc, const char* argv[]) {
    graph_access G;
    std::string graph_filename = argv[1];

    graph_io::readGraphWeighted(G, graph_filename);

    size_t num_nodes = 0;
    for(NodeID n = 0; n < G.number_of_nodes(); ++n) {
        ++num_nodes;
    }

    size_t num_edges = 0;
    for(NodeID n = 0; n < G.number_of_nodes(); ++n) {
        for (auto neighbour : G.neighbours(n)) {
            ++num_edges;
        }
    }

    if (num_edges != G.number_of_edges()) {
        std::cout << "Error!" << std::endl;
    }

    num_edges = 0;
    for(NodeID n = 0; n < G.number_of_nodes(); ++n) {
        num_edges += G.getNodeDegree(n);
    }

    if (num_edges != G.number_of_edges()) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << "Max degree = " << G.getMaxDegree() << std::endl;
}
