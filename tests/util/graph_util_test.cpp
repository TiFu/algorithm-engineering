#include <gtest/gtest.h>

#include "util/graph_util.h"
#include "data_structure/io/graph_io.h"

TEST(graph_util, toGraphvizStrig) {
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


TEST(graph_util, toGraphvizStrigWithColors) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    configuration_t s;
    s.resize(3);
    s[0].insert(0);
    s[0].insert(2);
    s[0].insert(4);
    s[1].insert(1);
    s[1].insert(3);
    s[2].insert(5);

    const char *expected_graphviz_str =
            "graph G {\n" \
        "    0 [penwidth=3 color=\"0 0.5 0.5\"];\n" \
        "    2 [penwidth=3 color=\"0 0.5 0.5\"];\n" \
        "    4 [penwidth=3 color=\"0 0.5 0.5\"];\n" \
        "    1 [penwidth=3 color=\"0.333 0.5 0.5\"];\n" \
        "    3 [penwidth=3 color=\"0.333 0.5 0.5\"];\n" \
        "    5 [penwidth=3 color=\"0.667 0.5 0.5\"];\n" \
        "    0 -- 1;\n" \
        "    0 -- 5;\n" \
        "    1 -- 2;\n" \
        "    1 -- 5;\n" \
        "    2 -- 3;\n" \
        "    3 -- 4;\n" \
        "    4 -- 5;\n" \
        "}\n";

    EXPECT_EQ(graph_util::toGraphvizStrig(G, s), expected_graphviz_str);
}

TEST(graph_util, toPartitionedGraphvizStrigWithColors) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    configuration_t s;
    s.resize(3);
    s[0].insert(0);
    s[0].insert(2);
    s[0].insert(4);
    s[1].insert(1);
    s[1].insert(3);
    s[2].insert(5);

    const char *expected_graphviz_str =
            "graph G {\n" \
            "    subgraph cluster_0 {\n" \
            "        0 [penwidth=3 color=\"0 0.5 0.5\"];\n" \
            "        2 [penwidth=3 color=\"0 0.5 0.5\"];\n" \
            "        4 [penwidth=3 color=\"0 0.5 0.5\"];\n" \
            "    }\n" \
            "    subgraph cluster_1 {\n" \
            "        1 [penwidth=3 color=\"0.333 0.5 0.5\"];\n" \
            "        3 [penwidth=3 color=\"0.333 0.5 0.5\"];\n" \
            "    }\n" \
            "    subgraph cluster_2 {\n" \
            "        5 [penwidth=3 color=\"0.667 0.5 0.5\"];\n" \
            "    }\n" \
            "    0 -- 1;\n" \
            "    0 -- 5;\n"\
            "    1 -- 2;\n" \
            "    1 -- 5;\n" \
            "    2 -- 3;\n"\
            "    3 -- 4;\n"\
            "    4 -- 5;\n"\
            "}\n";

    EXPECT_EQ(graph_util::toGraphvizStrig(G, s, true), expected_graphviz_str);
}