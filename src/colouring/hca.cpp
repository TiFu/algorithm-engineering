#include "colouring/hca.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

namespace graph_colouring {

    static bool cmpIllegalColoring(const graph_access &G,
                                   const Configuration &a,
                                   const Configuration &b) {
        return graph_colouring::numberOfConflictingEdges(G, a) >
               graph_colouring::numberOfConflictingEdges(G, b);
    }

    typedef std::function<Configuration(
            const std::vector<InitOperator> &initOperator,
            const std::vector<CrossoverOperator> &crossoverOperator,
            const std::vector<LSOperator> &lsOperator,
            const ConfugirationCompare &cmp,
            const graph_access &G,
            size_t k,
            size_t population_size,
            size_t maxItr)> ColouringAlgorithm;

    static Configuration hybridColouringAlgorithmTemplate(const ColouringAlgorithm &colouringAlgorithm,
                                                          const graph_access &G,
                                                          const size_t k,
                                                          const size_t population_size,
                                                          const size_t maxItr,
                                                          const size_t L,
                                                          const size_t A,
                                                          const double alpha) {
        InitOperator hcaInitOp = [L, A, alpha](const graph_access &graph,
                                               const size_t colors) {
            return graph_colouring::tabuSearchOperator(graph,
                                                       graph_colouring::initByGreedySaturation(graph, colors),
                                                       L,
                                                       A,
                                                       alpha);

        };

        CrossoverOperator hcaCrossoverOp = [](const graph_access &G_,
                                              const Configuration &s1,
                                              const Configuration &s2) {
            return graph_colouring::gpxCrossover(s1, s2);
        };

        LSOperator hcaLSOp = [L, A, alpha](const graph_access &graph,
                                           const Configuration &s) {
            return graph_colouring::tabuSearchOperator(graph, s, L, A, alpha);
        };

        return colouringAlgorithm({hcaInitOp},
                                  {hcaCrossoverOp},
                                  {hcaLSOp},
                                  cmpIllegalColoring,
                                  G,
                                  k,
                                  population_size,
                                  maxItr);
    }


    Configuration hybridColouringAlgorithm(const graph_access &G,
                                           size_t k,
                                           size_t population_size,
                                           size_t maxItr,
                                           size_t L,
                                           size_t A,
                                           double alpha) {
        return hybridColouringAlgorithmTemplate(colouringAlgorithm,
                                                G, k, population_size, maxItr, L, A, alpha);
    }

    Configuration parallelHybridColouringAlgorithm(const graph_access &G,
                                                   size_t k,
                                                   size_t population_size,
                                                   size_t maxItr,
                                                   size_t L,
                                                   size_t A,
                                                   double alpha) {
        return hybridColouringAlgorithmTemplate(parallelColouringAlgorithm,
                                                G, k, population_size, maxItr, L, A, alpha);
    }

}