#include <data_structure/graph.h>
#include <data_structure/io/graph_io.h>

#include <gtest/gtest.h>

TEST(GraphIO, readGraphWeighted) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";

    graph_io::readGraphWeighted(G, graph_filename);

    size_t num_nodes = 0;
    for(NodeID n = 0; n < G.number_of_nodes(); ++n) {
        ++num_nodes;
    }
    ASSERT_EQ(num_nodes, 6);

    size_t num_edges = 0;
    for(NodeID n = 0; n < G.number_of_nodes(); ++n) {
        for (auto neighbour : G.neighbours(n)) {
            ASSERT_TRUE(neighbour >= 0 && neighbour < num_nodes);
            ++num_edges;
        }
    }
    ASSERT_EQ(G.number_of_nodes(), num_nodes);

    num_edges = 0;
    for(NodeID n = 0; n < G.number_of_nodes(); ++n) {
        num_edges += G.getNodeDegree(n);
    }
    ASSERT_EQ(G.number_of_nodes(), num_nodes);

    EXPECT_EQ(G.getMaxDegree() , 3);
}
