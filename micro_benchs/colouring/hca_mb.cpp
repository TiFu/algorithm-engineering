#include "benchmark/benchmark.h"

#include "data_structure/io/graph_io.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

using namespace graph_colouring;

static bool cmpIllegalColoring(const graph_access &G,
                               const Configuration &a,
                               const Configuration &b) {
    return graph_colouring::numberOfConflictingEdges(G, a) >
           graph_colouring::numberOfConflictingEdges(G, b);
}

typedef std::function<Configuration(
        const InitOperator &initOperator,
        const CrossoverOperator &crossoverOperator,
        const LSOperator &lsOperator,
        const ConfugirationCompare &cmp,
        const graph_access &G,
        size_t k,
        size_t population_size,
        size_t maxItr)> ColouringAlgorithm;

Configuration testAlgorithm(const ColouringAlgorithm &colouringAlgorithm,
                              const graph_access &G,
                              const size_t k,
                              const size_t population_size,
                              const size_t maxItr,
                              const size_t L,
                              const size_t A,
                              const double alpha) {
    return colouringAlgorithm([L, A, alpha](const graph_access &graph,
                                            const size_t colors) {
        auto s_init = graph_colouring::initByGreedySaturation(graph, colors);
        return graph_colouring::tabuSearchOperator(graph,
                                                   s_init,
                                                   L,
                                                   A,
                                                   alpha);

    }, [](const graph_access &G_,
          const Configuration &s1,
          const Configuration &s2) {
        return graph_colouring::gpxCrossover(s1, s2);
    }, [L, A, alpha](const graph_access &graph,
                     const Configuration &s) {
        return graph_colouring::tabuSearchOperator(graph, s, L, A, alpha);
    }, cmpIllegalColoring, G, k, population_size, maxItr);
}


void BM_sequential(benchmark::State &state,
                   const char *graphFile) {
    while (state.KeepRunning()) {
        graph_access G;
        graph_io::readGraphWeighted(G, graphFile);

        const size_t L = 5;
        const size_t A = 2;
        const double alpha = 0.6;
        const size_t k = 8;
        const size_t population_size = 1000;
        const size_t maxItr = 20;
        testAlgorithm(coloringAlgorithm,
                      G, k, population_size, maxItr, L, A, alpha);
    }
}


void BM_parallel(benchmark::State &state,
                 const char *graphFile) {
    while (state.KeepRunning()) {
        graph_access G;
        graph_io::readGraphWeighted(G, graphFile);

        const size_t L = 5;
        const size_t A = 2;
        const double alpha = 0.6;
        const size_t k = 8;
        const size_t population_size = 1000;
        const size_t maxItr = 20;
        testAlgorithm(parallelColoringAlgorithm,
                      G, k, population_size, maxItr, L, A, alpha);
    }
}

BENCHMARK_CAPTURE(BM_sequential, miles250,
                  "../../input/miles250-sorted.graph")->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_parallel, miles250,
                  "../../input/miles250-sorted.graph")->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN()
