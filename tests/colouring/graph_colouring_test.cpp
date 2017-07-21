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
        //P[i] = generatePopulation(G, k, generator);
    }
    return P;
}

static void algorithm(const graph_access &G, const uint32_t k) {
    const uint32_t population_size = 10;

    //std::cerr << graph_util::toGraphvizStrig(G);

    population_t P = initPopulation(G, population_size, k);
}


TEST(graph_colouring, Testtest) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    //std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    algorithm(G, 2);
    //EXPECT_EQ(2, 3);
}
