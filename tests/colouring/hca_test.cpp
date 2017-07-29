#include "colouring/hca.h"

#include "data_structure/io/graph_io.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include <chrono>

TEST(HybridColouringAlgorithm, miles250_Graph_k8) {
    graph_access G;
    std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    const size_t L = 5;
    const size_t A = 2;
    const double alpha = 0.6;
    const size_t k = 8;
    const size_t population_size = 10;
    const size_t maxItr = 100;
    auto s_best = graph_colouring::hybridColouringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
    auto s_best_parallel = graph_colouring::parallelHybridColouringAlgorithm(G, k, population_size, maxItr, L, A,
                                                                             alpha);
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_best), 0);
    EXPECT_EQ(s_best, s_best_parallel);
}


TEST(HybridColouringAlgorithm, miles250_Graph_k7) {
    graph_access G;
    std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    const size_t L = 5;
    const size_t A = 2;
    const double alpha = 0.6;
    const size_t k = 7;
    const size_t population_size = 10;
    const size_t maxItr = 100;
    auto s_best = graph_colouring::hybridColouringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
    auto s_best_parallel = graph_colouring::parallelHybridColouringAlgorithm(G, k, population_size, maxItr, L, A,
                                                                             alpha);
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_best), 4);
    EXPECT_TRUE(graph_colouring::numberOfConflictingEdges(G, s_best_parallel) <= 4);
}
