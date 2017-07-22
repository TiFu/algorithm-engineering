#include "data_structure/io/graph_io.h"
#include "colouring/crossover/gpx.h"

#include "util/graph_util.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

TEST(gpx, gpxCrossover) {
    NodeID A = 0;
    NodeID B = 1;
    NodeID C = 2;
    NodeID D = 3;
    NodeID E = 4;
    NodeID F = 5;
    NodeID G = 6;
    NodeID H = 7;
    NodeID I = 8;
    NodeID J = 9;

    configuration_t s1;
    s1.resize(3);

    //Example from "Hybrid Evolutionary Algorithms for Graph Coloring"
    s1[0].insert(A);
    s1[0].insert(B);
    s1[0].insert(C);

    s1[1].insert(D);
    s1[1].insert(E);
    s1[1].insert(F);
    s1[1].insert(G);

    s1[2].insert(H);
    s1[2].insert(I);
    s1[2].insert(J);

    configuration_t s2;
    s2.resize(3);

    s2[0].insert(C);
    s2[0].insert(D);
    s2[0].insert(E);
    s2[0].insert(G);

    s2[1].insert(A);
    s2[1].insert(F);
    s2[1].insert(I);

    s2[2].insert(B);
    s2[2].insert(H);
    s2[2].insert(J);

    std::mt19937 generator;
    auto s = graph_colouring::gpxCrossover(s1, s2, generator);
    ASSERT_EQ(s.size(), s1.size());
    ASSERT_THAT(s[0], testing::ElementsAre(D, E, F, G));
    ASSERT_THAT(s[1], testing::ElementsAre(B, H, J));
    ASSERT_THAT(s[2], testing::ElementsAre(A, C, I));
}

