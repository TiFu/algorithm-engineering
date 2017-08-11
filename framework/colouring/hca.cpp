#include "hca.h"

#include "init/greedy_saturation.h"
#include "crossover/gpx.h"
#include "ls/tabu_search.h"

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

        auto invalidColoring = std::make_shared<FixedKColouringStrategy>();
        invalidColoring->initOperators.emplace_back([](const graph_access &graph,
                                                       const ColorCount colors) {
            return graph_colouring::initByGreedySaturation(graph, colors);

        });
        invalidColoring->crossoverOperators.emplace_back([](const Colouring &s1,
                                                            const Colouring &s2,
                                                            const graph_access &graph) {
            return graph_colouring::gpxCrossover(s1, s2);
        });
        invalidColoring->lsOperators.emplace_back([L, A, alpha](const Colouring &s,
                                                                const graph_access &graph) {
            return graph_colouring::tabuSearchOperator(s, graph, L, A, alpha);
        });

        return ColouringAlgorithm().perform({invalidColoring},
                                            G,
                                            k,
                                            population_size,
                                            maxItr,
                                            threadCount,
                                            outputStream)[0].s;
    }
}