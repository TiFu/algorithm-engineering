#include "colouring/hca.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

#include <algorithm>


namespace graph_colouring {

    configuration_t parallelHybridColoringAlgorithm(const graph_access &G,
                                                    const size_t k,
                                                    const size_t population_size,
                                                    const size_t maxItr,
                                                    const size_t L,
                                                    const size_t A,
                                                    const double alpha) {
        return graph_colouring::parallelColoringAlgorithm([L, A, alpha](const graph_access &graph,
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

}