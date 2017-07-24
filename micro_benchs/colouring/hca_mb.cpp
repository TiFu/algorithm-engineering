#include "benchmark/benchmark.h"

#include "data_structure/io/graph_io.h"

#include "colouring/init/greedy_saturation.h"
#include "colouring/crossover/gpx.h"
#include "colouring/ls/tabu_search.h"

#include <thread>

#include <algorithm>

using namespace graph_colouring;

typedef std::vector<configuration_t> population_t;

typedef std::function<configuration_t(
        const InitOperator &initOperator,
        const CrossoverOperator &crossoverOperator,
        const LSOperator &lsOperator,
        const graph_access &G,
        size_t k,
        size_t population_size,
        size_t maxItr)> ColouringAlgorithm;

configuration_t testAlgorithm(const ColouringAlgorithm &colouringAlgorithm,
                              const graph_access &G,
                              const size_t k,
                              const size_t population_size,
                              const size_t maxItr,
                              const size_t L,
                              const size_t A,
                              const double alpha) {
    return colouringAlgorithm([L, A, alpha](const graph_access &graph,
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


configuration_t parallel1(const InitOperator &initOperator,
                          const CrossoverOperator &crossoverOperator,
                          const LSOperator &lsOperator,
                          const graph_access &G,
                          const size_t k,
                          const size_t population_size,
                          const size_t maxItr) {

    std::vector<std::atomic_flag> lock(population_size);

    static std::mt19937 *generator;
#pragma omp threadprivate(generator)

    population_t P;
    P.resize(population_size);

#pragma omp parallel
    {
        if (generator == nullptr) {
            auto seed = std::hash<std::thread::id>()(std::this_thread::get_id());
            generator = new std::mt19937(seed);
        }

#pragma omp for
        for (size_t i = 0; i < population_size; i++) {
            P[i] = initOperator(G, k, *generator);
        }
    }


    for (size_t itr = 0; itr < maxItr; itr++) {
        const size_t mating_population_size = population_size / 2;
#pragma omp parallel
        {
            std::uniform_int_distribution<size_t> distribution(0, population_size - 1);
#pragma omp for
            for (size_t i = 0; i < mating_population_size; i++) {
                size_t p1 = distribution(*generator);
                for (auto j = 0; j < population_size; j++) {
                    auto nextElem = (p1 + j) % population_size;
                    if (lock[nextElem].test_and_set()) {
                        p1 = nextElem;
                        break;
                    }
                }
                size_t p2 = distribution(*generator);
                for (auto j = 0; j < population_size; j++) {
                    auto nextElem = (p2 + j) % population_size;
                    if (lock[nextElem].test_and_set()) {
                        p2 = nextElem;
                        break;
                    }
                }
                std::array<configuration_t *, 2> parents = {&P[2 * i], &P[2 * i + 1]};
                auto s = lsOperator(G,
                                    crossoverOperator(G, *parents[0], *parents[1], *generator),
                                    *generator);
                if (graph_colouring::numberOfConflictingEdges(G, *parents[0]) >
                    graph_colouring::numberOfConflictingEdges(G, *parents[1])) {
                    *parents[0] = s;
                } else {
                    *parents[1] = s;
                }
                lock[p1].clear();
                lock[p2].clear();
            }
        }
    }

    return *std::max_element(P.begin(),
                             P.end(),
                             [&G](const configuration_t &a, const configuration_t &b) {
                                 return graph_colouring::numberOfConflictingEdges(G, a) >
                                        graph_colouring::numberOfConflictingEdges(G, b);
                             });
}


void BM_sequential(benchmark::State &state,
                   const char *graphFile) {
    while (state.KeepRunning()) {
        graph_access G;
        graph_io::readGraphWeighted(G, graphFile);

        const size_t L = 5;
        const size_t A = 2;
        const double alpha = 0.6;
        const size_t k = 8;
        const size_t population_size = 1000;
        const size_t maxItr = 20;
        testAlgorithm(graph_colouring::parallelColoringAlgorithm,
                      G, k, population_size, maxItr, L, A, alpha);
    }
}


void BM_parallel(benchmark::State &state,
                 const char *graphFile) {
    while (state.KeepRunning()) {
        graph_access G;
        graph_io::readGraphWeighted(G, graphFile);

        const size_t L = 5;
        const size_t A = 2;
        const double alpha = 0.6;
        const size_t k = 8;
        const size_t population_size = 1000;
        const size_t maxItr = 20;
        testAlgorithm(parallel1,
                      G, k, population_size, maxItr, L, A, alpha);
    }
}

BENCHMARK_CAPTURE(BM_sequential, miles250,
                  "../../input/miles250-sorted.graph")->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_parallel, miles250,
                  "../../input/miles250-sorted.graph")->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN()
