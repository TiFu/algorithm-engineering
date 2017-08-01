#include "data_structure/graph.h"
#include "data_structure/io/graph_io.h"
#include "colouring/init/greedy_saturation.h"
#include "colouring/ls/tabu_search.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

TEST(GraphColouringTabuSearchOperator, SimpleGraph) {
    graph_access G;
    graph_io::readGraphWeighted(G, "../../input/simple.graph");

    graph_colouring::Colouring s_small_graph = {0, 2, 0, 1, 1, 0};

    auto s_small_graph_opt = graph_colouring::tabuSearchOperator(G, s_small_graph, 10, 3, 2);
    ASSERT_EQ(s_small_graph_opt[0], 0);
    ASSERT_EQ(s_small_graph_opt[1], 2);
    ASSERT_EQ(s_small_graph_opt[2], 0);
    ASSERT_EQ(s_small_graph_opt[3], 1);
    ASSERT_EQ(s_small_graph_opt[4], 2);
    ASSERT_EQ(s_small_graph_opt[5], 1);
}

TEST(GraphColouringTabuSearchOperator, Miles250Graph) {
    graph_access G;
    graph_io::readGraphWeighted(G, "../../input/miles250-sorted.graph");
    graph_colouring::Colouring s_init = graph_colouring::initByGreedySaturation(G, 5);
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, s_init), 71);

    auto s_opt = graph_colouring::tabuSearchOperator(G, s_init, 100, 3, 2);
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, s_opt), 16);
}
