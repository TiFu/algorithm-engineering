#include <gtest/gtest.h>

#include "util/graph_util.h"
#include "data_structure/graph.h"
#include "data_structure/io/graph_io.h"

TEST(GraphUtil, toGraphvizStrig) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    const char *expected_graphviz_str =
            "graph G {\n" \
            "    0 -- 1;\n" \
            "    0 -- 5;\n" \
            "    1 -- 2;\n" \
            "    1 -- 5;\n" \
            "    2 -- 3;\n" \
            "    3 -- 4;\n" \
            "    4 -- 5;\n" \
            "}\n";

    EXPECT_EQ(graph_util::toGraphvizStrig(G), expected_graphviz_str);
}
