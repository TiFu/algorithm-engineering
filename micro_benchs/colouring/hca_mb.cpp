#include "benchmark/benchmark.h"

#include "data_structure/io/graph_io.h"

#include "colouring/hca.h"

void BM_parallelHCA(benchmark::State &state,
                                    const char *graphFile) {
    while (state.KeepRunning()) {
        graph_access G;
        graph_io::readGraphWeighted(G, graphFile);

        const size_t L = 5;
        const size_t A = 2;
        const double alpha = 0.6;
        const size_t k = 8;
        const size_t population_size = 1000;
        const size_t maxItr = 10;
        graph_colouring::parallelHybridColoringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
    }
}

BENCHMARK_CAPTURE(BM_parallelHCA, miles250,
                  "../../input/miles250-sorted.graph")->Unit(benchmark::kMillisecond);

//BENCHMARK_CAPTURE(BM_parallelHCA, DSJC250_5,
//                  "../../input/DSJC250.5-sorted.graph")->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN()
