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

    inline size_t chooseParent(std::vector<std::atomic<bool>> &isFree,
                               size_t category,
                               size_t populationSize,
                               std::mt19937 &generator,
                               std::uniform_int_distribution<size_t> &distribution) {
        size_t nextTry;
        bool expected;
        do {
            expected = true;
            nextTry = category * populationSize + distribution(generator);
        } while (!isFree[nextTry].compare_exchange_weak(expected, false));
        return nextTry;
    }

    std::vector<ColouringResult>
    performColouringAlgorithmIteration(const std::vector<std::shared_ptr<ColouringStrategy> > &categories,
                                       const graph_access &G,
                                       const size_t k,
                                       const size_t populationSize,
                                       const size_t maxItr) {

        std::vector<Configuration> P(categories.size() * populationSize,
                                     Configuration(G.number_of_nodes()));
        //lock[i] = true -> i-th individual is free for mating
        std::vector<std::atomic<bool>> isFree(categories.size() * populationSize);

        #pragma omp parallel
        {
            typedef std::mt19937::result_type seed_type;
            typename std::chrono::system_clock seed_clock;
            auto init_seed = static_cast<seed_type>(seed_clock.now().time_since_epoch().count());
            init_seed += static_cast<seed_type>(omp_get_thread_num());
            std::mt19937 generator(init_seed);

            auto matingPopulationSize = populationSize / 2;
            std::uniform_int_distribution<size_t> distribution(0, populationSize - 1);

            std::vector<std::uniform_int_distribution<size_t>> initOprDists;
            initOprDists.reserve(categories.size());
            std::vector<std::uniform_int_distribution<size_t>> crossoverOprDists;
            crossoverOprDists.reserve(categories.size());
            std::vector<std::uniform_int_distribution<size_t>> lsOprDists;
            lsOprDists.reserve(categories.size());

            for (auto &category : categories) {
                initOprDists.emplace_back(0, category->initOperators.size() - 1);
                crossoverOprDists.emplace_back(0, category->crossoverOperators.size() - 1);
                lsOprDists.emplace_back(0, category->lsOperators.size() - 1);
            }

            //flag for a found solution.
            bool abort = false;

            #pragma omp for collapse(2)
            for (size_t j = 0; j < categories.size(); j++) {
                for (size_t i = 0; i < populationSize; i++) {
                    #pragma omp flush (abort)
                    if (!abort) {
                        P[j * populationSize + i] = categories[j]->initOperators[initOprDists[j](generator)](G, k);
                        isFree[j * populationSize + i] = true;
                        if (categories[j]->isSolution(G, k, P[j * populationSize + i])) {
                            abort = true;
                            #pragma omp flush (abort)
                        }
                    }
                }
            }

            if (!abort) {
                #pragma omp for collapse(3)
                for (size_t itr = 0; itr < maxItr; itr++) {
                    for (size_t categoryId = 0; categoryId < categories.size(); categoryId++) {
                        for (size_t matingPair = 0; matingPair < matingPopulationSize; matingPair++) {
                            #pragma omp flush (abort)
                            if (!abort) {
                                auto p1 = chooseParent(isFree, categoryId, populationSize, generator, distribution);
                                auto p2 = chooseParent(isFree, categoryId, populationSize, generator, distribution);

                                std::array<Configuration *, 2> parents = {&P[p1], &P[p2]};
                                auto weakerParent = static_cast<size_t>(categories[categoryId]->compare(G, *parents[0],
                                                                                                        *parents[1]));

                                auto crossoverOp = categories[categoryId]->crossoverOperators[
                                        crossoverOprDists[categoryId](generator)];
                                auto lsOp = categories[categoryId]->lsOperators[
                                        lsOprDists[categoryId](generator)];

                                *parents[weakerParent] = lsOp(G, crossoverOp(G, *parents[0], *parents[1]));

                                if (categories[categoryId]->isSolution(G, k, *parents[weakerParent])) {
                                    abort = true;
                                    #pragma omp flush (abort)
                                } else {
                                    isFree[p1] = true;
                                    isFree[p2] = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        std::vector<ColouringResult> bestResults(categories.size());
        for (size_t categoryId = 0; categoryId < categories.size(); categoryId++) {
            auto best = categoryId;
            for (int i = 0; i < populationSize; i++) {
                auto nextTry = categoryId * populationSize + i;
                if (categories[categoryId]->compare(G, P[best], P[nextTry])) {
                    best = nextTry;
                }
            }
            bestResults[categoryId] = {P[best], categories[categoryId]};
        }
        return bestResults;
    }

    std::vector<ColouringResult> colouringAlgorithm(const std::vector<std::shared_ptr<ColouringStrategy> > &categories,
                                                    const graph_access &G,
                                                    const size_t k,
                                                    const size_t populationSize,
                                                    const size_t maxItr,
                                                    const bool repeat,
                                                    std::ostream *outStream) {

        assert(!categories.empty());

        if (4 * omp_get_max_threads() > categories.size() * populationSize) {
            std::cerr << "WARNING: Make sure that populationSize is bigger than 4*numberCategories*numberCPUCores\n";
        }

        auto current_k = k;
        std::vector<ColouringResult> results;
        bool foundSolution;
        do {
            foundSolution = false;
            results = performColouringAlgorithmIteration(categories, G, current_k, populationSize, maxItr);

            for (auto &result : results) {
                if (result.category->isSolution(G, current_k, result.s)) {
                    foundSolution = true;
                    break;
                }
            }
            if (outStream != nullptr) {
                std::ostream &out = *outStream;
                for (auto &result : results) {
                    out << "###################################\n";
                    out << "Current k: " << current_k << "\n";
                    out << "Best Solutions:\n";
                    if (result.category->isSolution(G, current_k, result.s)) {
                        std::cout << " VALID  : ";
                    } else {
                        std::cout << " INVALID: ";
                    }
                    out << result.s << "\n";
                    out << "###################################\n";
                }
            }
            current_k--;
        } while (foundSolution && repeat);
        return results;
    }
}
