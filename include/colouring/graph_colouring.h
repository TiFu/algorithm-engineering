#pragma once

#include "data_structure/graph.h"

#include <functional>
#include <random>
#include <set>
#include <memory>

namespace graph_colouring {

    typedef std::vector<Color> Configuration;

    typedef std::function<Configuration(const graph_access &G,
                                        const size_t k)> InitOperator;

    typedef std::function<Configuration(const graph_access &G,
                                        const Configuration &s1,
                                        const Configuration &s2)> CrossoverOperator;

    typedef std::function<Configuration(const graph_access &G,
                                        const Configuration &s)> LSOperator;

    size_t colorCount(const Configuration &s);

    bool allowedInClass(const graph_access &G,
                        const Configuration &c,
                        Color color,
                        NodeID nodeID);

    size_t numberOfConflictingNodes(const graph_access &G,
                                    const Configuration &s);

    size_t numberOfConflictingEdges(const graph_access &G,
                                    const Configuration &s);

    inline size_t numberOfUncolouredNodes(const Configuration &s,
                                          const size_t k) {
        size_t count = 0;
        for (auto n : s) {
            assert(n == std::numeric_limits<Color>::max() || n < k);
            if (n == std::numeric_limits<Color>::max()) {
                count++;
            }
        }
        return count;
    }

    inline bool isFullyColoured(const Configuration &s,
                                const size_t k) {
        return numberOfUncolouredNodes(s, k) == 0;
    }

    class ColouringCategory {
    public:
        ColouringCategory(const std::vector<InitOperator> &initOperators,
                          const std::vector<CrossoverOperator> &crossoverOperators,
                          const std::vector<LSOperator> &lsOperators) :
                initOperators(initOperators),
                crossoverOperators(crossoverOperators),
                lsOperators(lsOperators) {
            assert(!initOperators.empty());
            assert(!crossoverOperators.empty());
            assert(!lsOperators.empty());
        }

        virtual ~ColouringCategory() = default;

        /**
         * True if the given coloring is a valid solution. By default, a valid coloring must:
         * - have (at most) k colors
         * - associates every node to a valid color (no partial colorings)
         * - has no conflicting edges within color clases
         * This function is called right after crossover and local search operation has been performed
         * on a given configuration \p s.
         * It is used to stop the current coloring algorithm
         * @param G the target graph
         * @param k the (maximum) number of colors
         * @param s the vertex coloring / configuration
         * @return true if the goven coloring is a valid solution.
         */
        virtual bool isSolution(const graph_access &G,
                                const size_t k,
                                const Configuration &s) {
            return colorCount(s) <= k &&
                   numberOfConflictingEdges(G, s) == 0 &&
                   isFullyColoured(s, k);
        }

        /**
         * Used to compare the scoring of two coloring.
         * @param G the target graph
         * @param a the first coloring
         * @param b the second coloring
         * @return True if coloring \p has a lesser score compared to coloring \p b
         */
        virtual bool compare(const graph_access &G,
                             const Configuration &a,
                             const Configuration &b) = 0;

        const std::vector<InitOperator> &initOperators;
        const std::vector<CrossoverOperator> &crossoverOperators;
        const std::vector<LSOperator> &lsOperators;
    };

    Configuration colouringAlgorithm(const std::vector<std::shared_ptr<ColouringCategory> > &categories,
                                     const graph_access &G,
                                     size_t k,
                                     size_t population_size,
                                     size_t maxItr);

    Configuration parallelColouringAlgorithm(const std::vector<std::shared_ptr<ColouringCategory> > &categories,
                                             const graph_access &G,
                                             size_t k,
                                             size_t population_size,
                                             size_t maxItr);

    class InvalidColouringCategory : public ColouringCategory {
    public:
        InvalidColouringCategory(const std::vector<InitOperator> &initOperators,
                                 const std::vector<CrossoverOperator> &crossoverOperators,
                                 const std::vector<LSOperator> &lsOperators) :
                ColouringCategory(initOperators, crossoverOperators, lsOperators) {
        }

        bool compare(const graph_access &G,
                     const Configuration &a,
                     const Configuration &b) override {
            return numberOfConflictingEdges(G, a) >
                   numberOfConflictingEdges(G, b);
        }
    };
}