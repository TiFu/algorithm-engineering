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

        const size_t min_k = 7;
        const size_t k = 9;
        const size_t population_size = 100;
        const size_t maxItr = 20;
        const size_t L = 5;
        const size_t A = 2;
        const double alpha = 0.6;
        auto result = hybridColouringAlgorithm(G, k, population_size, maxItr, L, A, alpha, 1);
        if(graph_colouring::colorCount(result) > min_k || graph_colouring::numberOfConflictingEdges(G, result) > 2) {
            std::cerr << "Should return a colouring with k=" << min_k << "\n";
        }
    }
}

void BM_parallel(benchmark::State &state,
                 const char *graphFile) {
    while (state.KeepRunning()) {
        graph_access G;
        graph_io::readGraphWeighted(G, graphFile);

        const size_t min_k = 7;
        const size_t k = 9;
        const size_t population_size = 100;
        const size_t maxItr = 20;
        const size_t L = 5;
        const size_t A = 2;
        const double alpha = 0.6;
        auto result = hybridColouringAlgorithm(G, k, population_size, maxItr, L, A, alpha);
        if(graph_colouring::colorCount(result) > min_k || graph_colouring::numberOfConflictingEdges(G, result) > 2) {
            std::cerr << "Should return a colouring with k=" << min_k << "\n";
        }
    }
}

BENCHMARK_CAPTURE(BM_sequential, miles250,
                  "../../input/miles250-sorted.graph")->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_parallel, miles250,
                  "../../input/miles250-sorted.graph")->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN()
