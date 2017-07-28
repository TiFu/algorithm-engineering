#include "colouring/hca.h"

#include "data_structure/io/graph_io.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include <chrono>

TEST(GraphColouringParallelHCA, miles250_Graph_k8) {
    graph_access G;
    std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    const size_t L = 5;
    const size_t A = 2;
    const double alpha = 0.6;
    const size_t k = 8;
    const size_t population_size = 10;
    const size_t maxItr = 100;
    auto s_best = graph_colouring::hybridColoringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_best), 0);
}


TEST(GraphColouringParallelHCA, miles250_Graph_k7) {
    graph_access G;
    std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    const size_t L = 5;
    const size_t A = 2;
    const double alpha = 0.6;
    const size_t k = 7;
    const size_t population_size = 10;
    const size_t maxItr = 100;
    auto s_best = graph_colouring::hybridColoringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_best), 4);
}
