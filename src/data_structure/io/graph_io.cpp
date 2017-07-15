#include "data_structure/io/graph_io.h"

int graph_io::readGraphWeighted(graph_access & G, std::string filename) {
    std::string line;

    // open file for reading
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "Error opening " << filename << std::endl;
        return 1;
    }

    NodeID nmbNodes;
    EdgeID nmbEdges;

    std::getline(in, line);
    //skip commentsm
    while ( line[0] == '%' ) {
        std::getline(in, line);
    }

    int ew = 0;
    std::stringstream ss(line);
    ss >> nmbNodes;
    ss >> nmbEdges;
    ss >> ew;

    if ( 2 * nmbEdges > std::numeric_limits<int>::max() || nmbNodes > std::numeric_limits<int>::max()) {
        std::cerr <<  "The graph is too large. Currently only 32bit supported!"  << std::endl;
        exit(0);
    }

    bool read_ew = false;
    bool read_nw = false;

    if (ew == 1) {
        read_ew = true;
    } else if (ew == 11) {
        read_ew = true;
        read_nw = true;
    } else if (ew == 10) {
        read_nw = true;
    }
    nmbEdges *= 2; //since we have forward and backward edges

    NodeID node_counter   = 0;
    EdgeID edge_counter   = 0;
    long long total_nodeweight = 0;

    G.start_construction(nmbNodes, nmbEdges);

    while (  std::getline(in, line)) {

        if (line[0] == '%') { // a comment in the file
            continue;
        }

        NodeID node = G.new_node();
        node_counter++;
        std::stringstream sstream(line);

        NodeID weight = 1;
        if ( read_nw ) {
            sstream >> weight;
            total_nodeweight += weight;
            if ( total_nodeweight > (long long) std::numeric_limits<NodeID>::max()) {
                std::cerr <<  "The sum of the node weights is too large (it exceeds the node weight type)."  << std::endl;
                std::cerr <<  "Currently not supported. Please scale your node weights."  << std::endl;
                exit(0);
            }
        }

        NodeID target;
        while ( sstream >> target ) {
            //check for self-loops
            if (target - 1 == node) {
                std::cerr <<  "The graph file contains self-loops. This is not supported. Please remove them from the file."  << std::endl;
            }

            EdgeID edge_weight = 1;
            if ( read_ew ) {
                sstream >> edge_weight;
            }
            edge_counter++;
            G.new_edge(node, target - 1);
        }

        if (in.eof()) {
            break;
        }
    }

    if ( edge_counter != (EdgeID) nmbEdges ) {
        std::cerr <<  "number of specified edges mismatch"  << std::endl;
        std::cerr <<  edge_counter <<  " " <<  nmbEdges  << std::endl;
        exit(0);
    }

    if ( node_counter != (NodeID) nmbNodes) {
        std::cerr <<  "number of specified nodes mismatch"  << std::endl;
        std::cerr <<  node_counter <<  " " <<  nmbNodes  << std::endl;
        exit(0);
    }


    G.finish_construction();
    return 0;
}
