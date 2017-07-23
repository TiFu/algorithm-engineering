#include "benchmark/benchmark.h"

#include "data_structure/io/graph_io.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

void BM_terrace_analysis__enumerate(benchmark::State &state,
                                    const char *newick_file,
                                    const char *data_file) {
    while (state.KeepRunning()) {
        graph_access G;
        //std::string graph_filename = "../../input/simple.graph";
        std::string graph_filename = "../../input/miles250-sorted.graph";
        //std::string graph_filename = "../../input/DSJC250.5-sorted.graph";
        graph_io::readGraphWeighted(G, graph_filename);
        //auto s_best = parallelHCA(G, 10, 10, 8);
        //std::cerr << "Best score:" << graph_colouring::numberOfConflictingEdges(G, s_best) << "\n";
    }
}

BENCHMARK_CAPTURE(BM_terrace_analysis__enumerate, Allium,
                  "../input/modified/Allium.nwk",
                  "../input/modified/Allium.data")->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN()
