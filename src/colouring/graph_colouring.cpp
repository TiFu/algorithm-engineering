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
        for (NodeID n = 0; n < s.size(); n++) {
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
        for (NodeID n = 0; n < s.size(); n++) {
            for (auto neighbour : G.neighbours(n)) {
                if (s[n] == s[neighbour]) {
                    count++;
                }
            }
        }
        return count / 2;
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

    Configuration colouringAlgorithm(const std::vector<std::shared_ptr<ColouringCategory> > &categories,
                                             const graph_access &G,
                                             const size_t k,
                                             const size_t population_size,
                                             const size_t maxItr) {

        std::vector<Configuration> P(population_size);

        //lock[i] = true -> i-th parent is free for mating
        std::vector<std::atomic<bool>> lock(population_size);

        if (4 * omp_get_max_threads() > population_size) {
            std::cerr << "WARNING: Make sure that population_size is bigger than 4*number_cores\n";
        }

#pragma omp parallel
        {
            typedef std::mt19937::result_type seed_type;
            typename std::chrono::system_clock seed_clock;
            auto mating_population_size = population_size / 2;
            auto init_seed = static_cast<seed_type>
            (seed_clock.now().time_since_epoch().count());
            init_seed += static_cast<seed_type>(omp_get_thread_num());
            std::mt19937 generator(init_seed);

            std::uniform_int_distribution<size_t> distribution(0, population_size - 1);
            std::uniform_int_distribution<size_t> initOprDist(0, categories[0]->initOperators.size() - 1);
            std::uniform_int_distribution<size_t> crossoverOprOprDist(0, categories[0]->crossoverOperators.size() - 1);
            std::uniform_int_distribution<size_t> lsOprOprDist(0, categories[0]->lsOperators.size() - 1);

#pragma omp for
            for (size_t i = 0; i < population_size; i++) {
                P[i] = categories[0]->initOperators[initOprDist(generator)](G, k);
                lock[i].store(true);
            }

#pragma omp for collapse(2) //schedule(guided)
            for (size_t itr = 0; itr < maxItr; itr++) {
                for (size_t i = 0; i < mating_population_size; i++) {
                    auto p1 = chooseParent(lock, generator, distribution);
                    auto p2 = chooseParent(lock, generator, distribution);

                    std::array<Configuration *, 2> parents = {&P[p1], &P[p2]};
                    auto weakerParent = static_cast<size_t>(categories[0]->compare(G, *parents[0], *parents[1]));

                    auto crossoverOp = categories[0]->crossoverOperators[crossoverOprOprDist(generator)];
                    auto lsOp = categories[0]->lsOperators[lsOprOprDist(generator)];

                    *parents[weakerParent] = lsOp(G, crossoverOp(G, *parents[0], *parents[1]));

                    lock[p1].store(true);
                    lock[p2].store(true);
                }
            }
        }
        return *std::max_element(P.begin(),
                                 P.end(),
                                 [&G, &categories](const Configuration &a, const Configuration &b) {
                                     return categories[0]->compare(G, a, b);
                                 });
    }
}