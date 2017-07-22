#include "data_structure/graph.h"
#include "data_structure/io/graph_io.h"
#include "colouring/graph_colouring.h"


#include "util/graph_util.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>


static population_t initPopulation(const graph_access &G, const size_t population_size, const uint32_t k) {
    population_t P;
    P.resize(population_size);
    std::mt19937 generator;

    for (size_t i = 0; i < population_size; i++) {
        //P[i] = initByGreedySaturation(G, k, generator);
    }
    return P;
}

static void algorithm(const graph_access &G, const uint32_t k) {
    const uint32_t population_size = 10;

    //std::cerr << graph_util::toGraphvizStrig(G);

    population_t P = initPopulation(G, population_size, k);
}

TEST(graph_colouring, numberOfConflictingEdges) {
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

TEST(graph_colouring, numberOfConflictingNodes) {
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

TEST(graph_colouring, Testtest) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    //std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    algorithm(G, 2);
    //EXPECT_EQ(2, 3);
}
