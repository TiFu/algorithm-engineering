#include "benchmark/benchmark.h"

#include "data_structure/io/graph_io.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

using namespace graph_colouring;

typedef std::vector<configuration_t> population_t;

typedef std::function<configuration_t(
        const InitOperator &initOperator,
        const CrossoverOperator &crossoverOperator,
        const LSOperator &lsOperator,
        const graph_access &G,
        size_t k,
        size_t population_size,
        size_t maxItr)> ColouringAlgorithm;

configuration_t testAlgorithm(const ColouringAlgorithm &colouringAlgorithm,
                              const graph_access &G,
                              const size_t k,
                              const size_t population_size,
                              const size_t maxItr,
                              const size_t L,
                              const size_t A,
                              const double alpha) {
    return colouringAlgorithm([L, A, alpha](const graph_access &graph,
                                            const size_t colors,
                                            std::mt19937 &generator) {
        auto s_init = graph_colouring::initByGreedySaturation(graph, colors, generator);
        return graph_colouring::tabuSearchOperator(graph,
                                                   s_init,
                                                   generator,
                                                   L,
                                                   A,
                                                   alpha);

    }, [](const graph_access &G_,
          const configuration_t &s1,
          const configuration_t &s2,
          std::mt19937 &generator) {
        return graph_colouring::gpxCrossover(s1, s2, generator);
    }, [L, A, alpha](const graph_access &graph,
                     const configuration_t &s,
                     std::mt19937 &generator) {
        return graph_colouring::tabuSearchOperator(graph, s, generator, L, A, alpha);
    }, G, k, population_size, maxItr);
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
