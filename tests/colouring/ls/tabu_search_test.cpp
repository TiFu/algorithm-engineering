#include "data_structure/graph.h"
#include "data_structure/io/graph_io.h"
#include "colouring/init/greedy_saturation.h"
#include "colouring/ls/tabu_search.h"

#include "util/graph_util.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

TEST(graph_colouring, tabuSearchOperator) {
    std::mt19937 generator;

    graph_access G;
    graph_io::readGraphWeighted(G, "../../input/simple.graph");
    configuration_t s_small_graph;
    s_small_graph.resize(3);
    s_small_graph[0].insert(0);
    s_small_graph[0].insert(1);
    s_small_graph[0].insert(2);
    s_small_graph[1].insert(3);
    s_small_graph[1].insert(4);
    s_small_graph[1].insert(5);

    auto s_small_graph_opt = graph_colouring::tabuSearchOperator(G, s_small_graph, 10, 3, 2, generator);
    ASSERT_THAT(s_small_graph_opt[0], testing::ElementsAre(0, 2));
    ASSERT_THAT(s_small_graph_opt[1], testing::ElementsAre(3, 5));
    ASSERT_THAT(s_small_graph_opt[2], testing::ElementsAre(1, 4));
}

TEST(graph_colouring, tabuSearchOperator_miles250_sorted) {
    std::mt19937 generator;

    graph_access G;
    graph_io::readGraphWeighted(G, "../../input/miles250-sorted.graph");
    configuration_t s_init = graph_colouring::initByGreedySaturation(G, 5, generator);
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, s_init), 71);

    auto s_opt = graph_colouring::tabuSearchOperator(G, s_init, 100, 3, 2, generator);
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, s_opt), 16);
}