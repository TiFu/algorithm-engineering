#include "colouring/graph_colouring.h"

#include "data_structure/io/graph_io.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include <debug.h>

using namespace graph_colouring;

TEST(GraphColouringNumberOfConflictingEdges, SimpleGraph) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    graph_colouring::Colouring s_zero_score = {0, 1, 0, 1, 0, 2};
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_zero_score), 0);

    graph_colouring::Colouring s_two_classes_score = {0, 0, 0, 0, 0, 1};
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_two_classes_score), 4);

    graph_colouring::Colouring s_worst_score = {0, 0, 0, 0, 0, 0};
    EXPECT_EQ(graph_colouring::numberOfConflictingEdges(G, s_worst_score), 7);
}

TEST(GraphColouringNumberOfConflictingNodes, SimpleGraph) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    graph_colouring::Colouring s_zero_score = {0, 1, 0, 1, 0, 2};
    EXPECT_EQ(graph_colouring::numberOfConflictingNodes(G, s_zero_score), 0);

    graph_colouring::Colouring s_two_classes_score = {0, 0, 0, 0, 0, 1};
    EXPECT_EQ(graph_colouring::numberOfConflictingNodes(G, s_two_classes_score), 5);

    graph_colouring::Colouring s_worst_score = {0, 0, 0, 0, 0, 0};
    EXPECT_EQ(graph_colouring::numberOfConflictingNodes(G, s_worst_score), 6);
}

TEST(GraphColouring, parallelSchedule) {


    auto invalidColoring = std::make_shared<FixedKColouringStrategy>();
    invalidColoring->initOperators.emplace_back([](const graph_access &graph,
                                                   const size_t colors) {
        Colouring dummy(colors);
        return dummy;
    });
    invalidColoring->initOperators.emplace_back([](const graph_access &graph,
                                                   const size_t colors) {
        Colouring dummy(colors);
        return dummy;
    });
    invalidColoring->initOperators.emplace_back([](const graph_access &graph,
                                                   const size_t colors) {
        Colouring dummy(colors);
        return dummy;
    });
    invalidColoring->crossoverOperators.emplace_back([](const Colouring &s1,
                                                        const Colouring &s2,
                                                        const graph_access &graph) {
        return s1;
    });
    invalidColoring->crossoverOperators.emplace_back([](const Colouring &s1,
                                                        const Colouring &s2,
                                                        const graph_access &graph) {
        return s1;
    });
    invalidColoring->lsOperators.emplace_back([](const Colouring &s,
                                                 const graph_access &graph) {
        return s;
    });

    auto validColoring = std::make_shared<VariableColouringStrategy>();

    validColoring->initOperators.emplace_back([](const graph_access &graph,
                                                 const size_t colors) {
        Colouring dummy(colors);
        return dummy;
    });

    size_t executionCounter = 0;
    validColoring->crossoverOperators.emplace_back([&executionCounter](
            const Colouring &s1,
            const Colouring &s2,
            const graph_access &graph) {
        executionCounter++;
        if (executionCounter > 100) {
            Colouring solution(s1.size());
            for (Color i = 0; i < solution.size(); i++) {
                solution[i] = i;
            }
            return solution;
        }
        return s1;
    });
    validColoring->lsOperators.emplace_back([](const Colouring &s,
                                               const graph_access &graph) {
        return s;
    });

    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    const size_t k = 5;
    const size_t populationSize = 20;
    const size_t maxItr = 100;

    ColouringAlgorithm().perform({invalidColoring, validColoring},
                                 G,
                                 k,
                                 populationSize,
                                 maxItr);
    //ASSERT_TRUE(hcaCrossoverOp1Count > 0 && hcaCrossoverOp1Count < maxItr * population_size / 2);
}