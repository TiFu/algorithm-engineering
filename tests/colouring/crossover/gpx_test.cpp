#include "data_structure/io/graph_io.h"
#include "colouring/crossover/gpx.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

TEST(GraphColouringGPX, Simple) {
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

    graph_colouring::Configuration s1(10);

    //Example from "Hybrid Evolutionary Algorithms for Graph Coloring"
    s1[A] = 0;
    s1[B] = 0;
    s1[C] = 0;

    s1[D] = 1;
    s1[E] = 1;
    s1[F] = 1;
    s1[G] = 1;

    s1[H] = 2;
    s1[I] = 2;
    s1[J] = 2;

    graph_colouring::Configuration s2(10);

    s2[C] = 0;
    s2[D] = 0;
    s2[E] = 0;
    s2[G] = 0;

    s2[A] = 1;
    s2[F] = 1;
    s2[I] = 1;

    s2[B] = 2;
    s2[H] = 2;
    s2[J] = 2;

    auto s = graph_colouring::gpxCrossover(s1, s2);

    ASSERT_EQ(s.size(), s1.size());

    ASSERT_EQ(s[D], 0);
    ASSERT_EQ(s[E], 0);
    ASSERT_EQ(s[F], 0);
    ASSERT_EQ(s[G], 0);

    ASSERT_EQ(s[B], 1);
    ASSERT_EQ(s[H], 1);
    ASSERT_EQ(s[J], 1);

    ASSERT_EQ(s[A], 2);
    ASSERT_EQ(s[C], 2);
    ASSERT_EQ(s[I], 2);
}

