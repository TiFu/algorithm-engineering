#include "colouring/hca.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

namespace graph_colouring {

    Colouring hybridColouringAlgorithm(
            const graph_access &G,
            const ColorCount k,
            const size_t population_size,
            const size_t maxItr,
            const size_t L,
            const size_t A,
            const double alpha,
            const size_t threadCount,
            std::ostream *outputStream) {

        std::vector<InitOperator> hcaInitOps = {[](const graph_access &graph,
                                                   const ColorCount colors) {
            return graph_colouring::initByGreedySaturation(graph, colors);

        }};
        std::vector<CrossoverOperator> hcaCrossoverOps = {[](const Colouring &s1,
                                                             const Colouring &s2,
                                                             const graph_access &graph) {
            return graph_colouring::gpxCrossover(s1, s2);
        }};
        std::vector<LSOperator> hcaLSOps = {[L, A, alpha](const Colouring &s,
                                                          const graph_access &graph) {
            return graph_colouring::tabuSearchOperator(s, graph, L, A, alpha);
        }};

        auto invalidColoring = std::make_shared<FixedKColouringStrategy>(hcaInitOps,
                                                                          hcaCrossoverOps,
                                                                          hcaLSOps);

        return ColouringAlgorithm().perform({invalidColoring},
                                            G,
                                            k,
                                            population_size,
                                            maxItr,
                                            threadCount,
                                            outputStream)[0].s;
    }
}