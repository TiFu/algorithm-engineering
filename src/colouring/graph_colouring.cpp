#include "colouring/graph_colouring.h"

#include <atomic>
#include <algorithm>
#include <debug.h>

namespace graph_colouring {

    /**
     * Used to encapsulate a independently processable working package
     * within the thread pool of the genetic algorithm.
     */
    struct WorkingPackage {
        /**< The current iteration cont.
         * If itr=0, the working thread will initialize colourig1 using a initialization operator
         * If itr>0, the working thread will perform regular crossover and ls operator
         */
        size_t itr;
        /**< The used colouring strategy */
        size_t strategyId;
        /**< The number of colors used for the colouring strategy */
        size_t target_k;
        /**< The index of the first colouring */
        size_t colouring;
    };

    /**
     * Used to notify the master thread about certain events
     */
    struct MasterPackage {
        /**< The next k to be searched */
        size_t next_k;
    };

    /**
     * Used in the parallel colouring algorithm to track certain events
     */
    struct ColouringStrategyContext {
        /**> Counts the number of finished pedigrees (=populationSize/2) */
        std::atomic<size_t> wpCount;
    };

    size_t colorCount(const Colouring &s) {
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
                        const Colouring &c,
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
                                    const Colouring &s) {
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
                                    const Colouring &s) {
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

    inline size_t chooseParent(const size_t strategyId,
                               const size_t populationSize,
                               std::vector<std::atomic<bool>> &lock,
                               std::mt19937 &generator) {
        std::uniform_int_distribution<size_t> populationDist(0, populationSize - 1);
        size_t nextTry;
        bool expected;
        do {
            expected = false;
            nextTry = strategyId * populationSize + populationDist(generator);
        } while (!lock[nextTry].compare_exchange_weak(expected, true));
        return nextTry;
    }

    static void workerThread(const std::vector<std::shared_ptr<ColouringStrategy>> &strategies,
                             const graph_access &G,
                             const size_t populationSize,
                             const size_t maxItr,
                             const size_t threadId,
                             std::vector<ColouringStrategyContext> &context,
                             boost::lockfree::queue<WorkingPackage> &workQueue,
                             boost::lockfree::queue<MasterPackage> &masterQueue,
                             std::vector<Colouring> &population,
                             std::vector<std::atomic<bool>> &lock,
                             std::atomic<size_t> &target_k,
                             std::atomic<bool> &terminated) {
        typedef std::mt19937::result_type seed_type;
        typename std::chrono::system_clock seed_clock;
        auto init_seed = static_cast<seed_type>(seed_clock.now().time_since_epoch().count());
        init_seed += static_cast<seed_type>(threadId);

        std::mt19937 generator(init_seed);

        //Only used to avoid rapid reporting of already known colourings
        size_t last_reported_k = target_k + 1;

        WorkingPackage wp = {0, 0, 0, 0};
        while (!terminated) {
            while (workQueue.pop(wp)) {
                const ColouringStrategy &strategy = *strategies[wp.strategyId];

                if (target_k < wp.target_k && strategy.isFixedKStrategy()) {
                    context[wp.strategyId].wpCount.fetch_sub(1);
                    continue;
                }

                if (wp.itr > 0) {
                    auto p1 = chooseParent(wp.strategyId, populationSize, lock, generator);
                    auto p2 = chooseParent(wp.strategyId, populationSize, lock, generator);

                    std::array<Colouring *, 2> parents = {&population[p1], &population[p2]};
                    auto weakerParent = static_cast<size_t>(strategy.compare(G,
                                                                             *parents[0],
                                                                             *parents[1]));

                    std::uniform_int_distribution<size_t> crossoverOprDist(0,
                                                                           strategy.crossoverOperators.size() - 1);
                    std::uniform_int_distribution<size_t> lsOprDist(0, strategy.lsOperators.size() - 1);

                    auto crossoverOp = strategy.crossoverOperators[
                            crossoverOprDist(generator)];
                    auto lsOp = strategies[wp.strategyId]->lsOperators[
                            lsOprDist(generator)];

                    auto &target = *parents[weakerParent];

                    target = lsOp(G, crossoverOp(G, *parents[0], *parents[1]));

                    if (strategy.isSolution(G, target_k, target) && last_reported_k >= target_k) {
                        last_reported_k = colorCount(target);
                        masterQueue.push({last_reported_k});
                    }

                    lock[p1] = false;
                    lock[p2] = false;

                    if (wp.itr < maxItr) {
                        workQueue.push({wp.itr + 1, wp.strategyId, wp.target_k, wp.colouring});
                    } else {
                        context[wp.strategyId].wpCount.fetch_sub(1);
                    }
                } else {
                    std::uniform_int_distribution<size_t> initOprDist(0, strategy.initOperators.size() - 1);
                    std::uniform_int_distribution<size_t> lsOprDist(0, strategy.lsOperators.size() - 1);

                    auto initOpr = strategy.initOperators[initOprDist(generator)];
                    auto lsOpr = strategy.lsOperators[lsOprDist(generator)];

                    auto &target = population[wp.strategyId * populationSize + wp.colouring];

                    target = lsOpr(G, initOpr(G, wp.target_k));

                    if (strategy.isSolution(G, target_k, target) && last_reported_k > target_k) {
                        last_reported_k = colorCount(target);
                        masterQueue.push({last_reported_k});
                    }

                    lock[wp.strategyId * populationSize + wp.colouring] = false;

                    auto matingPopulationSize = populationSize / 2;
                    if (wp.colouring < matingPopulationSize) {
                        workQueue.push({wp.itr + 1, wp.strategyId, wp.target_k, wp.colouring});
                    } else {
                        context[wp.strategyId].wpCount.fetch_sub(1);
                    }
                }
            }
            std::this_thread::yield();
        }
    }

    bool hasFinished(const std::vector<ColouringStrategyContext> &context) {
        for (auto &c : context) {
            if (c.wpCount > 0) {
                return false;
            }
        }
        return true;
    }

    std::vector<ColouringResult>
    ColouringAlgorithm::perform(const std::vector<std::shared_ptr<ColouringStrategy>> &strategies,
                                const graph_access &G,
                                const size_t k,
                                const size_t populationSize,
                                const size_t maxItr,
                                const size_t threadCount) {

        assert(!strategies.empty());
        assert(populationSize > 0);
        assert(maxItr > 0);
        assert(threadCount > 0);

        if (4 * threadCount > strategies.size() * populationSize) {
            std::cerr << "WARNING: Make sure that populationSize is bigger than 4*categoryCount*threadCount\n";
        }

        std::vector<Colouring> P(strategies.size() * populationSize);
        //lock[i] = true -> i-th individual is free for mating
        std::vector<std::atomic<bool>> lock(strategies.size() * populationSize);
        std::vector<ColouringStrategyContext> context(strategies.size());

        //Represents the smallest number of colors used in a recently found colouring
        std::atomic<size_t> target_k(k);
        boost::lockfree::queue<WorkingPackage> workQueue(populationSize * strategies.size() * k);

        //It is possible that all threads report the same found k colouring
        boost::lockfree::queue<MasterPackage> masterQueue(k * threadCount);

        //Used to signal the termination of the worker pool
        std::atomic<bool> terminated(false);

        std::vector<std::thread> workerPool;
        workerPool.reserve(threadCount);
        for (size_t threadId = 0; threadId < threadCount; threadId++) {
            workerPool.emplace_back(workerThread,
                                    std::cref(strategies),
                                    std::cref(G),
                                    populationSize,
                                    maxItr,
                                    threadId,
                                    std::ref(context),
                                    std::ref(workQueue),
                                    std::ref(masterQueue),
                                    std::ref(P),
                                    std::ref(lock),
                                    std::ref(target_k),
                                    std::ref(terminated));
        }

        //Init work queue
        for (size_t colouringId = 0; colouringId < populationSize; colouringId++) {
            for (size_t strategyId = 0; strategyId < strategies.size(); strategyId++) {
                lock[strategyId * populationSize + colouringId] = true;
                context[strategyId].wpCount.fetch_add(1);
                workQueue.push({0, strategyId, target_k, colouringId});
            }
        }

        MasterPackage mp = {0};
        while (!hasFinished(context)) {
            while (masterQueue.pop(mp)) {
                target_k = mp.next_k - 1;
                for (size_t strategyId = 0; strategyId < strategies.size(); strategyId++) {
                    if (strategies[strategyId]->isFixedKStrategy()) {
                        //Wait until every worker stopped working on the effected population
                        while (context[strategyId].wpCount > 0) {
                            std::this_thread::yield();
                        }
                        context[strategyId].wpCount = 0;
                        for (size_t colouringId = 0; colouringId < populationSize; colouringId++) {
                            lock[strategyId * populationSize + colouringId] = true;
                        }
                        for (size_t colouringId = 0; colouringId < populationSize; colouringId++) {
                            context[strategyId].wpCount.fetch_add(1);
                            workQueue.push({0, strategyId, target_k, colouringId});
                        }
                    }
                }
            }
            std::this_thread::yield();
        }
        terminated = true;

        for (auto &worker : workerPool) {
            worker.join();
        }

        std::vector<ColouringResult> bestResults(strategies.size());
        for (size_t strategyId = 0; strategyId < strategies.size(); strategyId++) {
            auto best = strategyId;
            for (int i = 0; i < populationSize; i++) {
                auto nextTry = strategyId * populationSize + i;
                if (strategies[strategyId]->compare(G, P[best], P[nextTry])) {
                    best = nextTry;
                }
            }
            bestResults[strategyId] = {P[best], strategies[strategyId]};
        }
        return bestResults;
    }
}
