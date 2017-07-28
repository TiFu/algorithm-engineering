#include "colouring/graph_colouring.h"

#include "data_structure/io/graph_io.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include <chrono>

TEST(GraphColouringNumberOfConflictingEdges, SimpleGraph) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    graph_colouring::Configuration s_zero_score = {0, 1, 0, 1, 0, 2};
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_zero_score), 0);

    graph_colouring::Configuration s_two_classes_score = {0, 0, 0, 0, 0, 1};
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_two_classes_score), 4);

    graph_colouring::Configuration s_worst_score = {0, 0, 0, 0, 0, 0};
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_worst_score), 7);
}

TEST(GraphColouringNumberOfConflictingNodes, SimpleGraph) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    graph_colouring::Configuration s_zero_score = {0, 1, 0, 1, 0, 2};
    EXPECT_EQ(graph_colouring::numberOfConflictingNodes(G, s_zero_score), 0);

    graph_colouring::Configuration s_two_classes_score = {0, 0, 0, 0, 0, 1};
    EXPECT_EQ(graph_colouring::numberOfConflictingNodes(G, s_two_classes_score), 5);

    graph_colouring::Configuration s_worst_score = {0, 0, 0, 0, 0, 0};
    EXPECT_EQ(graph_colouring::numberOfConflictingNodes(G, s_worst_score), 6);
}
