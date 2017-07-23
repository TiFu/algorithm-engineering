#include "data_structure/io/graph_io.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

#include "util/graph_util.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include <chrono>

typedef std::vector<configuration_t> population_t;

static population_t initPopulation(const graph_access &G,
                                   const size_t population_size,
                                   const size_t k,
                                   std::mt19937 generator) {
    population_t P;
    P.resize(population_size);

    for (size_t i = 0; i < population_size; i++) {
        auto s_init = graph_colouring::initByGreedySaturation(G, k, generator);
        P[i] = graph_colouring::tabuSearchOperator(G,
                                                   s_init,
                                                   generator,
                                                   5,
                                                   2,
                                                   0.6);
    }
    return P;
}

static configuration_t parallelHCA(const graph_access &G,
                                   const size_t k,
                                   const size_t population_size,
                                   const size_t maxItr) {
    std::mt19937 generator;

    population_t P = initPopulation(G, population_size, k, generator);

    for (size_t itr = 0; itr < maxItr; itr++) {
        const size_t mating_population_size = population_size / 2;
        std::uniform_int_distribution<size_t> distribution(0, population_size - 1);
        for (size_t i = 0; i < mating_population_size; i++) {
            std::array<configuration_t *, 2> parents = {&P[distribution(generator)], &P[distribution(generator)]};
            auto s = graph_colouring::tabuSearchOperator(G, graph_colouring::gpxCrossover(*parents[0],
                                                                                          *parents[1],
                                                                                          generator),
                                                         generator,
                                                         5,
                                                         2,
                                                         0.6);
            if (graph_colouring::numberOfConflictingEdges(G, *parents[0]) >
                graph_colouring::numberOfConflictingEdges(G, *parents[1])) {
                *parents[0] = s;
            } else {
                *parents[1] = s;
            }
        }
    }
    return *std::max_element(P.begin(),
                             P.end(),
                             [&G](const configuration_t &a, const configuration_t &b) {
                                 return graph_colouring::numberOfConflictingEdges(G, a) >
                                        graph_colouring::numberOfConflictingEdges(G, b);
                             });
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

    auto begin = std::chrono::high_resolution_clock::now();
    auto s_best = parallelHCA(G, 8, 10, 10);
    auto end = std::chrono::high_resolution_clock::now();
    std::cerr << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1000.0 / 1000.0 / 1000.0
              << "s" << std::endl;

    std::cerr << "Best score:" << graph_colouring::numberOfConflictingEdges(G, s_best) << "\n";
    //EXPECT_EQ(2, 3);
}
