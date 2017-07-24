#include "data_structure/io/graph_io.h"

#include "colouring/hca.h"

#include "util/graph_util.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include <chrono>

TEST(GraphColouringNumberOfConflictingEdges, SimpleGraph) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    configuration_t s_zero_score;
    s_zero_score.resize(3);
    s_zero_score[0].insert(0);
    s_zero_score[0].insert(2);
    s_zero_score[0].insert(4);
    s_zero_score[1].insert(1);
    s_zero_score[1].insert(3);
    s_zero_score[2].insert(5);
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_zero_score), 0);

    configuration_t s_two_classes_score;
    s_two_classes_score.resize(3);
    s_two_classes_score[0].insert(0);
    s_two_classes_score[0].insert(1);
    s_two_classes_score[0].insert(2);
    s_two_classes_score[0].insert(3);
    s_two_classes_score[0].insert(4);
    s_two_classes_score[1].insert(5);
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_two_classes_score), 4);

    configuration_t s_worst_score;
    s_worst_score.resize(3);
    for (NodeID n = 0; n < G.number_of_nodes(); ++n) {
        s_worst_score[0].insert(n);
    }
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_worst_score), 7);
}

TEST(GraphColouringNumberOfConflictingNodes, SimpleGraph) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    configuration_t s_zero_score;
    s_zero_score.resize(3);
    s_zero_score[0].insert(0);
    s_zero_score[0].insert(2);
    s_zero_score[0].insert(4);
    s_zero_score[1].insert(1);
    s_zero_score[1].insert(3);
    s_zero_score[2].insert(5);
    EXPECT_EQ(graph_colouring::numberOfConflictingNodes(G, s_zero_score), 0);

    configuration_t s_two_classes_score;
    s_two_classes_score.resize(3);
    s_two_classes_score[0].insert(0);
    s_two_classes_score[0].insert(1);
    s_two_classes_score[0].insert(2);
    s_two_classes_score[0].insert(3);
    s_two_classes_score[0].insert(4);
    s_two_classes_score[1].insert(5);
    EXPECT_EQ(graph_colouring::numberOfConflictingNodes(G, s_two_classes_score), 5);

    configuration_t s_worst_score;
    s_worst_score.resize(3);
    for (NodeID n = 0; n < G.number_of_nodes(); ++n) {
        s_worst_score[0].insert(n);
    }
    EXPECT_EQ(graph_colouring::numberOfConflictingNodes(G, s_worst_score), 6);
}

TEST(GraphColouringParallelHCA, DSJC250_5_Graph) {
    graph_access G;
    //std::string graph_filename = "../../input/simple.graph";
    std::string graph_filename = "../../input/miles250-sorted.graph";
    //std::string graph_filename = "../../input/DSJC250.5-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    const size_t L = 5;
    const size_t A = 2;
    const double alpha = 0.6;
    const size_t k = 8;
    const size_t population_size = 10;
    const size_t maxItr = 10;
    auto s_best = graph_colouring::hybridColoringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_best), 0);
}
