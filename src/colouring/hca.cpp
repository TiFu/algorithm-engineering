#include "colouring/hca.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

namespace graph_colouring {

    typedef std::function<Configuration(
            const std::vector<std::shared_ptr<ColouringCategory>> &categories,
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


        auto invalidColoring = std::make_shared<InvalidColouringCategory>(hcaInitOps,
                                                                          hcaCrossoverOps,
                                                                          hcaLSOps);

        return colouringAlgorithm({invalidColoring},
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