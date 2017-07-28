#include "colouring/hca.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

#include <algorithm>


namespace graph_colouring {

    Configuration hybridColoringAlgorithm(const graph_access &G,
                                            size_t k,
                                            size_t population_size,
                                            size_t maxItr,
                                            size_t L,
                                            size_t A,
                                            double alpha) {
        return graph_colouring::coloringAlgorithm([L, A, alpha](const graph_access &graph,
                                                                const size_t colors) {
            auto s_init = graph_colouring::initByGreedySaturation(graph, colors);
            return graph_colouring::tabuSearchOperator(graph,
                                                       s_init,
                                                       L,
                                                       A,
                                                       alpha);

        }, [](const graph_access &G_,
              const Configuration &s1,
              const Configuration &s2) {
            return graph_colouring::gpxCrossover(s1, s2);
        }, [L, A, alpha](const graph_access &graph,
                         const Configuration &s) {
            return graph_colouring::tabuSearchOperator(graph, s, L, A, alpha);
        }, G, k, population_size, maxItr);
    }

}