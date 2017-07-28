#include "colouring/init/greedy_saturation.h"
#include "data_structure/io/graph_io.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

TEST(GraphColouringGreedySaturation, SimpleGraph) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    auto s_init = graph_colouring::initByGreedySaturation(G,3);

    EXPECT_EQ(s_init.size(), G.number_of_nodes());
    ASSERT_EQ(s_init[0], 0);
    ASSERT_EQ(s_init[1], 1);
    ASSERT_EQ(s_init[2], 0);
    ASSERT_EQ(s_init[3], 1);
    ASSERT_EQ(s_init[4], 0);
    ASSERT_EQ(s_init[5], 2);
}