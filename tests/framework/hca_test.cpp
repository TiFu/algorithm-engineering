#include "data_structure/graph.h"
#include "data_structure/io/graph_io.h"
#include "hca/hca.h"

#include <gtest/gtest.h>

static void hca(const graph_access &G, const uint32_t k) {

}

TEST(Test, Testtest) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";

    graph_io::readGraphWeighted(G, graph_filename);
    EXPECT_EQ(2, 3);
}
