#include "benchmark/benchmark.h"

#include "data_structure/io/graph_io.h"
#include "util/graph_util.h"
#include "colouring/hca.h"

using namespace graph_colouring;


void BM_hca(benchmark::State &state,
            const char *graphFile) {
    while (state.KeepRunning()) {
        graph_access G;
        graph_io::readGraphWeighted(G, graphFile);

        auto threadCount = size_t(state.range(0));
        auto min_k = ColorCount(state.range(1));
        auto k = ColorCount(state.range(2));
        auto population_size = size_t(state.range(3));
        auto maxItr = size_t(state.range(4));
        auto L = size_t(state.range(5));
        auto A = size_t(state.range(6));
        const double alpha = double(state.range(7)) / 10;
        auto result = hybridColouringAlgorithm(G, k, population_size, maxItr, L, A, alpha, threadCount);
        auto result_k = graph_colouring::colorCount(result.s);
        if (result_k > min_k) {
            std::cerr << "Should return a colouring with k = "
                      << min_k << " (is actually " << result_k << ")\n";
        }
        if (!result.isValid) {
            std::cerr << "Should return a valid colouring\n";
        }
    }
}

BENCHMARK_CAPTURE(BM_hca, miles250,
                  "../../input/miles250-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->Args({1, 8, 9, 100,  20, 5, 2, 6})
        ->Args({2, 8, 9, 100,  20, 5, 2, 6})
        ->Args({1, 8, 9, 1000, 20, 5, 2, 6})
        ->Args({2, 8, 9, 1000, 20, 5, 2, 6});

/*
BENCHMARK_CAPTURE(BM_hca, DSJC250_5,
                  "../../input/DSJC250.5-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->Args({1, 28, 30, 100, 20, 250, 2, 6})
        ->Args({2, 28, 30, 100, 20, 250, 2, 6});
*/

BENCHMARK_MAIN()
