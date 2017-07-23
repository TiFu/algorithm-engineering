#include "data_structure/io/graph_io.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

#include "util/graph_util.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include <chrono>

static configuration_t parallelHCA(const graph_access &G,
                                   const size_t k,
                                   const size_t population_size,
                                   const size_t maxItr,
                                   const size_t L,
                                   const size_t A,
                                   const double alpha) {
    return graph_colouring::parallelColoringAlgorithm([L, A, alpha](const graph_access &graph,
                                                                    const size_t colors,
                                                                    std::mt19937 &generator) {
        auto s_init = graph_colouring::initByGreedySaturation(graph, colors, generator);
        return graph_colouring::tabuSearchOperator(graph,
                                                   s_init,
                                                   generator,
                                                   L,
                                                   A,
                                                   alpha);

    }, [](const graph_access &G_,
          const configuration_t &s1,
          const configuration_t &s2,
          std::mt19937 &generator) {
        return graph_colouring::gpxCrossover(s1, s2, generator);
    }, [L, A, alpha](const graph_access &graph,
                     const configuration_t &s,
                     std::mt19937 &generator) {
        return graph_colouring::tabuSearchOperator(graph, s, generator, L, A, alpha);
    }, G, k, population_size, maxItr);
}

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
    auto begin = std::chrono::high_resolution_clock::now();
    auto s_best = parallelHCA(G, k, population_size, maxItr, L, A, alpha);
    auto end = std::chrono::high_resolution_clock::now();
    std::cerr << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1000.0 / 1000.0 / 1000.0
              << "s" << std::endl;
    std::cerr << "Best score:" << graph_colouring::numberOfConflictingEdges(G, s_best) << "\n";
    //EXPECT_EQ(2, 3);
}
