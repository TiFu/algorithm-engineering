#include <debug.h>
#include "benchmark/benchmark.h"

#include "data_structure/io/graph_io.h"
#include "util/graph_util.h"
#include "colouring/hca.h"

using namespace graph_colouring;

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
        auto result = hybridColouringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
        assert(graph_colouring::numberOfConflictingEdges(G, result) == 0);
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
        auto result = parallelHybridColouringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
        assert(graph_colouring::numberOfConflictingEdges(G, result) == 0);
    }
}

BENCHMARK_CAPTURE(BM_sequential, miles250,
                  "../../input/miles250-sorted.graph")->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_parallel, miles250,
                  "../../input/miles250-sorted.graph")->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN()
