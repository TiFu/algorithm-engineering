#pragma once

#include "data_structure/graph.h"

#include <functional>
#include <random>
#include <set>
#include <memory>

template<typename T>
std::ostream &operator<<(std::ostream &strm, const std::set<T> &set) {
    strm << "{";
    bool first = true;
    for (const T &elem : set) {
        if (first) {
            first = false;
        } else {
            strm << ",";
        }
        strm << elem;
    }
    return strm << "}";
}

template<typename T>
std::ostream &operator<<(std::ostream &strm, const std::vector<T> &set) {
    strm << "[";
    bool first = true;
    for (const T &elem : set) {
        if (first) {
            first = false;
        } else {
            strm << ",";
        }
        strm << elem;
    }
    return strm << "]";
}

namespace graph_colouring {

    /**
     * Represents a colouring of a specific graph where
     * config[n] = c means that color c is associated to node with node id n
     * Use config[n] = std::numeric_limits<Color>::max() to mark node n as uncoloured
     */
    typedef std::vector<Color> Configuration;

    /**
     * Operator used to create an initial configuration with k colors.
     * The parameter k represents the (desired) number of colors for the configuration.
     */
    typedef std::function<Configuration(const graph_access &G,
                                        const size_t k)> InitOperator;

    /**
     * Creates a new colouring based on two existing parent configurations s1 and s2.
     */
    typedef std::function<Configuration(const graph_access &G,
                                        const Configuration &s1,
                                        const Configuration &s2)> CrossoverOperator;

    /**
     * Optimizes / mutates the existign colouring s.
     */
    typedef std::function<Configuration(const graph_access &G,
                                        const Configuration &s)> LSOperator;

    /**
     * @param s a graph colouring
     * @return the number of colurs used in the configuration \s
     */
    size_t colorCount(const Configuration &s);

    /**
     * @param G the target graph
     * @param s the colouring of graph \p G
     * @param color the color class to test
     * @param nodeID the node to test
     * @return true if the node denoted by \p noteID has
     * edges to nodes with the same color class
     */
    bool allowedInClass(const graph_access &G,
                        const Configuration &s,
                        Color color,
                        NodeID nodeID);

    /**
     * @param G the target graph
     * @param s the colouring of graph \p G
     * @return the number of nodes whose neighbours reside in the same color class
     */
    size_t numberOfConflictingNodes(const graph_access &G,
                                    const Configuration &s);

    /**
     *
     * @param G the target graph
     * @param s the colouring of graph \p G
     * @return the number of edges between two nodes that have the same colouring
     */
    size_t numberOfConflictingEdges(const graph_access &G,
                                    const Configuration &s);

    /**
     * @param s a colouring
     * @return the number of nodes thate have no associated color
     */
    inline size_t numberOfUncolouredNodes(const Configuration &s) {
        size_t count = 0;
        for (auto n : s) {
            assert(n == std::numeric_limits<Color>::max() || n < k);
            if (n == std::numeric_limits<Color>::max()) {
                count++;
            }
        }
        return count;
    }

    /**
     * @param s a colouring
     * @return true if all nodes in the given configuration do have colors
     */
    inline bool isFullyColoured(const Configuration &s) {
        return numberOfUncolouredNodes(s) == 0;
    }


    /**
     * This class is used to group operates together which follow a common colouring strategy.
     * Such a colouring strategy can be:
     * - Producing / Enhancing invalid colourings
     * - Producing / Enhancing valid colourings with possibly more then k colours
     * - Producing / Enhancing valid, but partial colourings.
     * Every colouring strategy can provide its own comparison operator for comparing two different colourings and
     * validation method.
     */
    class ColouringStrategy {
    public:
        ColouringStrategy(const std::vector<InitOperator> &initOperators,
                          const std::vector<CrossoverOperator> &crossoverOperators,
                          const std::vector<LSOperator> &lsOperators) :
                initOperators(initOperators),
                crossoverOperators(crossoverOperators),
                lsOperators(lsOperators) {
            assert(!initOperators.empty());
            assert(!crossoverOperators.empty());
            assert(!lsOperators.empty());
        }

        virtual ~ColouringStrategy() = default;

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
         * @return true if the given coloring is a valid solution.
         */
        virtual bool isSolution(const graph_access &G,
                                const size_t k,
                                const Configuration &s) {
            return colorCount(s) <= k &&
                   numberOfConflictingEdges(G, s) == 0 &&
                   isFullyColoured(s);
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

        /**< Used initialization operators */
        const std::vector<InitOperator> &initOperators;
        /**< Crossover operators */
        const std::vector<CrossoverOperator> &crossoverOperators;
        /**< Local Search / Mutation operators */
        const std::vector<LSOperator> &lsOperators;
    };

    /**
     * This strategy is supposed for operator families which can handle invalid colourings
     */
    class InvalidColouringStrategy : public ColouringStrategy {
    public:
        InvalidColouringStrategy(const std::vector<InitOperator> &initOperators,
                                 const std::vector<CrossoverOperator> &crossoverOperators,
                                 const std::vector<LSOperator> &lsOperators) :
                ColouringStrategy(initOperators, crossoverOperators, lsOperators) {
        }

        bool compare(const graph_access &G,
                     const Configuration &a,
                     const Configuration &b) override {
            return numberOfConflictingEdges(G, a) >
                   numberOfConflictingEdges(G, b);
        }
    };

    /**
     * @brief Represents a single result from coloingAlgorithm()
     */
    struct ColouringResult {
        /**< The best configuration */
        Configuration s;
        /**< The algorithm category used to retrive the corresponding category*/
        std::shared_ptr<ColouringStrategy> category;
    };

    /**
     * The main entry point for executing the genetic algorithm in parallel.
     * It will maintain a polulation for each colouring category passed to this function.
     * Each colouring category represent a specific strategy and therefore provides its own sets of
     * operators and scoring functions.
     * @param categories the categories of operators and scoring functions used in this run
     * @param G the target graph
     * @param k the (maximum) number of colors
     * @param populationSize the number of maintained colourings
     * @param maxItr the maximum number of iterations
     * @param repeat if true, the algorithm will repeat the entire execution
     * with a smaller value for k (k-1, k-2, k-3, ...) as long as a valid solution
     * for k-i coloring could be obtained in the previous runs
     * @param outStream
     * @return the best colourings for each passed colouring category
     */
    std::vector<ColouringResult>
    colouringAlgorithm(const std::vector<std::shared_ptr<ColouringStrategy> > &categories,
                       const graph_access &G,
                       size_t k,
                       size_t populationSize,
                       size_t maxItr,
                       bool repeat = true,
                       std::ostream *outStream = nullptr);

}