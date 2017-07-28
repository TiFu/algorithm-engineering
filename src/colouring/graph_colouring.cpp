#include "colouring/graph_colouring.h"

#include <atomic>
#include <algorithm>
#include <thread>
#include <omp.h>

namespace graph_colouring {

    size_t colorCount(const configuration_t &s) {
        std::vector<bool> usedColor(s.size());
        size_t color_count = 0;
        for (auto n : s) {
            //max for partial colorings
            if (n != std::numeric_limits<Color>::max() && !usedColor[n]) {
                usedColor[n] = true;
                color_count++;
            }
        }
        return color_count;
    }

    bool allowedInClass(const graph_access &G,
                        const configuration_t &c,
                        const Color color,
                        const NodeID nodeID) {
        for (auto neighbour : G.neighbours(nodeID)) {
            if (c[neighbour] == color) {
                return false;
            }
        }
        return true;
    }

    size_t numberOfConflictingNodes(const graph_access &G,
                                    const configuration_t &s) {
        size_t count = 0;
        for (size_t n = 0; n < s.size(); n++) {
            for (auto neighbour : G.neighbours(n)) {
                if (s[n] == s[neighbour]) {
                    count++;
                    break;
                }
            }
        }
        return count;
    }

    size_t numberOfConflictingEdges(const graph_access &G,
                                    const configuration_t &s) {
        size_t count = 0;
        for (size_t n = 0; n < s.size(); n++) {
            for (auto neighbour : G.neighbours(n)) {
                if (s[n] == s[neighbour]) {
                    count++;
                }
            }
        }
        return count / 2;
    }


    configuration_t coloringAlgorithm(const InitOperator &initOperator,
                                      const CrossoverOperator &crossoverOperator,
                                      const LSOperator &lsOperator,
                                      const graph_access &G,
                                      size_t k,
                                      size_t population_size,
                                      size_t maxItr) {
        std::vector<configuration_t> P(population_size);

        for (size_t i = 0; i < population_size; i++) {
            P[i] = initOperator(G, k);
        }

        std::mt19937 generator;
        for (size_t itr = 0; itr < maxItr; itr++) {
            const size_t mating_population_size = population_size / 2;
            std::uniform_int_distribution<size_t> distribution(0, population_size - 1);
            for (size_t i = 0; i < mating_population_size; i++) {
                std::array<configuration_t *, 2> parents = {&P[distribution(generator)], &P[distribution(generator)]};

                auto scoreP1 = graph_colouring::numberOfConflictingEdges(G, *parents[0]);
                auto scoreP2 = graph_colouring::numberOfConflictingEdges(G, *parents[1]);

                size_t target = 1 - static_cast<size_t>(scoreP1 > scoreP2);

                *parents[target] = lsOperator(G,
                                              crossoverOperator(G, *parents[0], *parents[1]));
            }
        }
        return *std::max_element(P.begin(),
                                 P.end(),
                                 [&G](const configuration_t &a, const configuration_t &b) {
                                     return graph_colouring::numberOfConflictingEdges(G, a) >
                                            graph_colouring::numberOfConflictingEdges(G, b);
                                 });
    }

    inline size_t chooseParent(std::vector<std::atomic<bool>> &lock,
                               std::mt19937 &generator,
                               std::uniform_int_distribution<size_t> &distribution) {
        size_t nextTry;
        bool expected;
        do {
            expected = true;
            nextTry = distribution(generator);
        } while (!lock[nextTry].compare_exchange_weak(expected, false));
        return nextTry;
    }

    typedef std::mt19937::result_type seed_type;

    typename std::chrono::system_clock seed_clock;

    configuration_t parallelColoringAlgorithm(const InitOperator &initOperator,
                                              const CrossoverOperator &crossoverOperator,
                                              const LSOperator &lsOperator,
                                              const graph_access &G,
                                              const size_t k,
                                              const size_t population_size,
                                              const size_t maxItr) {

        std::vector<configuration_t> P;
        P.resize(population_size);

        //lock[i] = true -> i-th parent is free for mating
        std::vector<std::atomic<bool>> lock(population_size);

#pragma omp parallel for
        for (size_t i = 0; i < population_size; i++) {
            P[i] = initOperator(G, k);
            lock[i].store(true);
        }

        const size_t mating_population_size = population_size / 2;
        for (size_t itr = 0; itr < maxItr; itr++) {
#pragma omp parallel
            {
                auto init_seed = static_cast<seed_type>
                (seed_clock.now().time_since_epoch().count());
                init_seed += static_cast<seed_type>(omp_get_thread_num());
                std::mt19937 generator(init_seed);
                std::uniform_int_distribution<size_t> distribution(0, population_size - 1);
#pragma omp for
                for (size_t i = 0; i < mating_population_size; i++) {
                    auto p1 = chooseParent(lock, generator, distribution);
                    auto p2 = chooseParent(lock, generator, distribution);
                    std::array<configuration_t *, 2> parents = {&P[p1],
                                                                &P[p2]};
                    auto scoreP1 = graph_colouring::numberOfConflictingEdges(G, *parents[0]);
                    auto scoreP2 = graph_colouring::numberOfConflictingEdges(G, *parents[1]);

                    size_t target = 1 - static_cast<size_t>(scoreP1 > scoreP2);

                    *parents[target] = lsOperator(G,
                                                  crossoverOperator(G, *parents[0], *parents[1]));
                    lock[p1].store(true);
                    lock[p2].store(true);
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
}