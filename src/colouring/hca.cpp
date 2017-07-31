#include "colouring/hca.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

namespace graph_colouring {

    Configuration hybridColouringAlgorithm(
            const graph_access &G,
            const size_t k,
            const size_t population_size,
            const size_t maxItr,
            const size_t L,
            const size_t A,
            const double alpha,
            bool repeat,
            std::ostream *logStream) {
        std::vector<InitOperator> hcaInitOps = {[L, A, alpha](const graph_access &graph,
                                                              const size_t colors) {
            return graph_colouring::tabuSearchOperator(graph,
                                                       graph_colouring::initByGreedySaturation(graph, colors),
                                                       L,
                                                       A,
                                                       alpha);

        }};
        std::vector<CrossoverOperator> hcaCrossoverOps = {[](const graph_access &G_,
                                                             const Configuration &s1,
                                                             const Configuration &s2) {
            return graph_colouring::gpxCrossover(s1, s2);
        }};
        std::vector<LSOperator> hcaLSOps = {[L, A, alpha](const graph_access &graph,
                                                          const Configuration &s) {
            return graph_colouring::tabuSearchOperator(graph, s, L, A, alpha);
        }};

        auto invalidColoring = std::make_shared<InvalidColouringStrategy>(hcaInitOps,
                                                                          hcaCrossoverOps,
                                                                          hcaLSOps);

        return colouringAlgorithm({invalidColoring},
                                  G,
                                  k,
                                  population_size,
                                  maxItr,
                                  repeat,
                                  logStream)[0].s;
    }
}