#include "colouring/hca.h"

#include "data_structure/io/graph_io.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include <chrono>
#include <colouring/init/greedy_saturation.h>
#include <colouring/crossover/gpx.h>
#include <colouring/ls/tabu_search.h>

using namespace graph_colouring;

TEST(HybridColouringAlgorithm, miles250_Graph_k7) {
    graph_access G;
    std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    const size_t L = 5;
    const size_t A = 2;
    const double alpha = 0.6;
    const size_t k = 7;
    const size_t population_size = 20;
    const size_t maxItr = 20;
    auto s_best = graph_colouring::hybridColouringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
    EXPECT_EQ(graph_colouring::colorCount(s_best), 7);
    EXPECT_TRUE(graph_colouring::numberOfConflictingEdges(G, s_best) <= 4);
}
