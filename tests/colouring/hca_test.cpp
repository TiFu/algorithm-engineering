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

TEST(HybridColouringAlgorithm, miles250_Graph_random_selection) {
    graph_access G;
    std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    const size_t L = 5;
    const size_t A = 2;
    const double alpha = 0.6;
    const size_t k = 7;
    const size_t population_size = 20;
    const size_t maxItr = 100;

    std::vector<InitOperator> hcaInitOps = {[L, A, alpha](const graph_access &graph,
                                                          const size_t colors) {
        return tabuSearchOperator(graph,
                                  initByGreedySaturation(graph, colors),
                                  L,
                                  A,
                                  alpha);

    }};

    size_t hcaCrossoverOp1Count = 0;
    size_t hcaCrossoverOp2Count = 0;

    std::vector<CrossoverOperator> hcaCrossoverOps = {
            [&hcaCrossoverOp1Count](const graph_access &G_,
                                    const Configuration &s1,
                                    const Configuration &s2) {
                hcaCrossoverOp1Count++;
                return gpxCrossover(s1, s2);
            }, [&hcaCrossoverOp2Count](const graph_access &G_,
                                       const Configuration &s1,
                                       const Configuration &s2) {
                hcaCrossoverOp2Count++;
                return gpxCrossover(s1, s2);
            }};

    std::vector<LSOperator> hcaLSOps = {[L, A, alpha](const graph_access &graph,
                                                      const Configuration &s) {
        return tabuSearchOperator(graph, s, L, A, alpha);
    }};

    auto invalidColoring = std::make_shared<InvalidColouringStrategy>(hcaInitOps,
                                                                      hcaCrossoverOps,
                                                                      hcaLSOps);

    colouringAlgorithm({invalidColoring},
                       G,
                       k,
                       population_size,
                       maxItr);
    ASSERT_TRUE(hcaCrossoverOp1Count > 0 && hcaCrossoverOp1Count < maxItr * population_size / 2);
    ASSERT_TRUE(hcaCrossoverOp2Count > 0 && hcaCrossoverOp2Count < maxItr * population_size / 2);
}


TEST(HybridColouringAlgorithm, miles250_Graph_k8) {
    graph_access G;
    std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    const size_t L = 5;
    const size_t A = 2;
    const double alpha = 0.6;
    const size_t k = 8;
    const size_t population_size = 20;
    const size_t maxItr = 100;
    auto s_best = graph_colouring::hybridColouringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_best), 0);
}


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
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_best), 2);
}
