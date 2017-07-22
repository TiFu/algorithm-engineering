#include "colouring/init/greedy_saturation.h"
#include "data_structure/io/graph_io.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

TEST(GraphColouringGreedySaturation, SimpleGraph) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    std::mt19937 generator;

    auto s_init = graph_colouring::initByGreedySaturation(G,3, generator);

    EXPECT_EQ(s_init.size(), 3);
    EXPECT_THAT(s_init[0], testing::ElementsAre(0, 2, 4));
    EXPECT_THAT(s_init[1], testing::ElementsAre(1, 3));
    EXPECT_THAT(s_init[2], testing::ElementsAre(5));
}