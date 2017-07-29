#include "colouring/graph_colouring.h"

#include <atomic>
#include <algorithm>
#include <thread>
#include <omp.h>

namespace graph_colouring {

    size_t colorCount(const Configuration &s) {
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
                        const Configuration &c,
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
                                    const Configuration &s) {
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
                                    const Configuration &s) {
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


    Configuration coloringAlgorithm(const InitOperator &initOperator,
                                    const CrossoverOperator &crossoverOperator,
                                    const LSOperator &lsOperator,
                                    const ConfugirationCompare &cmp,
                                    const graph_access &G,
                                    size_t k,
                                    size_t population_size,
                                    size_t maxItr) {
        std::vector<Configuration> P(population_size);
        std::mt19937 generator;
        const size_t mating_population_size = population_size / 2;
        std::uniform_int_distribution<size_t> distribution(0, population_size - 1);

        for (size_t i = 0; i < population_size; i++) {
            P[i] = initOperator(G, k);
        }

        for (size_t itr = 0; itr < maxItr; itr++) {
            for (size_t i = 0; i < mating_population_size; i++) {
                std::array<Configuration *, 2> parents = {&P[distribution(generator)], &P[distribution(generator)]};

                auto weakerParent = static_cast<size_t>(cmp(G, *parents[0], *parents[1]));

                *parents[weakerParent] = lsOperator(G,
                                                    crossoverOperator(G, *parents[0], *parents[1]));
            }
        }
        return *std::max_element(P.begin(),
                                 P.end(),
                                 [&G, &cmp](const Configuration &a, const Configuration &b) {
                                     return cmp(G, a, b);
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

    Configuration parallelColoringAlgorithm(const InitOperator &initOperator,
                                            const CrossoverOperator &crossoverOperator,
                                            const LSOperator &lsOperator,
                                            const ConfugirationCompare &cmp,
                                            const graph_access &G,
                                            const size_t k,
                                            const size_t population_size,
                                            const size_t maxItr) {

        std::vector<Configuration> P;
        P.resize(population_size);

        //lock[i] = true -> i-th parent is free for mating
        std::vector<std::atomic<bool>> lock(population_size);

#       pragma omp parallel
        {
            auto mating_population_size = population_size / 2;
            auto init_seed = static_cast<seed_type>
            (seed_clock.now().time_since_epoch().count());
            init_seed += static_cast<seed_type>(omp_get_thread_num());
            std::mt19937 generator(init_seed);
            std::uniform_int_distribution<size_t> distribution(0, population_size - 1);

            #pragma omp for
            for (size_t i = 0; i < population_size; i++) {
                P[i] = initOperator(G, k);
                lock[i].store(true);
            }

            #pragma omp for collapse(2)
            for (size_t itr = 0; itr < maxItr; itr++) {
                for (size_t i = 0; i < mating_population_size; i++) {
                    auto p1 = chooseParent(lock, generator, distribution);
                    auto p2 = chooseParent(lock, generator, distribution);

                    std::array<Configuration *, 2> parents = {&P[p1], &P[p2]};
                    auto weakerParent = static_cast<size_t>(cmp(G, *parents[0], *parents[1]));
                    *parents[weakerParent] = lsOperator(G,
                                                        crossoverOperator(G, *parents[0], *parents[1]));

                    lock[p1].store(true);
                    lock[p2].store(true);
                }
            }
        }

        return *std::max_element(P.begin(),
                                 P.end(),
                                 [&G, &cmp](const Configuration &a, const Configuration &b) {
                                     return cmp(G, a, b);
                                 });
    }
}