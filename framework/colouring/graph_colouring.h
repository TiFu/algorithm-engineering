#pragma once

#include "../../data_structure/graph.h"

#include <functional>
#include <random>
#include <set>
#include <memory>
#include <thread>
#include <boost/lockfree/queue.hpp>
#include <cstdint>

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

    typedef Color ColorCount;

    /**
     * Represents a colouring of a specific graph where
     * config[n] = c means that color c is associated to node with node id n
     * Use config[n] = UNCOLORED or config[n] = std::numeric_limits<Color>::max()
     * to mark node n as uncoloured
     */
    typedef std::vector<Color> Colouring;

    /**
     * Operator used to create an initial configuration with k colors.
     * The parameter k represents the (desired) number of colors for the configuration.
     */
    typedef std::function<Colouring(const graph_access &G,
                                    const ColorCount k)> InitOperator;

    /**
     * Creates a new colouring based on two existing parent configurations s1 and s2.
     */
    typedef std::function<Colouring(const Colouring &s1,
                                    const Colouring &s2,
                                    const graph_access &G)> CrossoverOperator;

    /**
     * Optimizes / mutates the existign colouring s.
     */
    typedef std::function<Colouring(const Colouring &s,
                                    const graph_access &G)> LSOperator;

    /**
     * @param s a graph colouring
     * @return the number of colurs used in the configuration \s
     */
    ColorCount colorCount(const Colouring &s);

    /**
     * @param s a graph colouring
     * @return the sum of squared color class sizes used in the configuration times -1 \s
     */
    int64_t squaredColorClassSizes(const Colouring &s);

    /**
     * @param G the target graph
     * @param s the colouring of graph \p G
     * @param color the color class to test
     * @param nodeID the node to test
     * @return true if the node denoted by \p noteID has
     * edges to nodes with the same color class
     */
    bool allowedInClass(const graph_access &G,
                        const Colouring &s,
                        Color color,
                        NodeID nodeID);

    /**
     * @param G the target graph
     * @param s the colouring of graph \p G
     * @return the number of nodes whose neighbours reside in the same color class
     */
    size_t numberOfConflictingNodes(const graph_access &G,
                                    const Colouring &s);

    /**
     *
     * @param G the target graph
     * @param s the colouring of graph \p G
     * @return the number of edges between two nodes that have the same colouring
     */
    size_t numberOfConflictingEdges(const graph_access &G,
                                    const Colouring &s);

    /**
     *
     * @param G the target graph
     * @param s the colouring of graph \p G
     * @return the sum of the degrees of the vertices that are uncolored
     */
    size_t sumUncoloredDegree(const graph_access &G,
                                    const Colouring &s);

    /**
     * @param s a colouring
     * @return the number of nodes thate have no associated color
     */
    inline size_t numberOfUncolouredNodes(const Colouring &s) {
        size_t count = 0;
        for (auto n : s) {
            if (n == UNCOLORED) {
                count++;
            }
        }
        return count;
    }

    /**
     * @param s a colouring
     * @return true if all nodes in the given configuration do have colors
     */
    inline bool isFullyColoured(const Colouring &s) {
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
                                const ColorCount k,
                                const Colouring &s) const {
            return colorCount(s) <= k &&
                   numberOfConflictingEdges(G, s) == 0 &&
                   isFullyColoured(s);
        }

        /**
         * @return true if this colouring strategy is supposed to use a fixed number of
         * colourings. The ColouringAlgorithm will re-start the genetic algorithm on the populations
         * induced by this strategy if a valid colouring with less or equal specified colors has ben found.
         */
        virtual bool isFixedKStrategy() const = 0;

        /**
         * Used to compare the scorings of two colorings within a strategy-specific population.
         * It is guaranteed that the compared instances resulted from the same initialization phase,
         * which may be important in fixed-k strategies.
         * The executing parallel colouring algorithm will always prefer the colourings with the smallest
         * amount of used colours for the final reporting.
         * @param G the target graph
         * @param a the first coloring
         * @param b the second coloring
         * @return True if coloring \p a has a lesser score compared to coloring \p b
         */
        virtual bool compare(const graph_access &G,
                             const Colouring &a,
                             const Colouring &b) const = 0;

        /**< Used initialization operators */
        std::vector<InitOperator> initOperators;
        /**< Crossover operators */
        std::vector<CrossoverOperator> crossoverOperators;
        /**< Local Search / Mutation operators */
        std::vector<LSOperator> lsOperators;
    };

    /**
     * This strategy is supposed for operator families which can handle invalid colourings
     */
    class FixedKColouringStrategy : public ColouringStrategy {
    public:
        bool compare(const graph_access &G,
                     const Colouring &a,
                     const Colouring &b) const override {
            return numberOfConflictingEdges(G, a) >
                   numberOfConflictingEdges(G, b);
        }

        bool isFixedKStrategy() const override {
            return true;
        }
    };

    /**
     * This strategy is supposed for operator families which can handle invalid colourings
     */
    class FixedKPartialColouringStrategy : public ColouringStrategy {
    public:
        bool compare(const graph_access &G,
                     const Colouring &a,
                     const Colouring &b) const override {
            return sumUncoloredDegree(G, a) >
                sumUncoloredDegree(G, b);
        }

        bool isFixedKStrategy() const override {
            return true;
        }
    };

    /**
     * This strategy is supposed for operator families which can handle valid colourings
     */
    class VariableColouringStrategy : public ColouringStrategy {
    public:
        bool compare(const graph_access &G,
                     const Colouring &a,
                     const Colouring &b) const override {
            return squaredColorClassSizes(a) >
                   squaredColorClassSizes(b);
        }

        bool isFixedKStrategy() const override {
            return false;
        }
    };

    /**
     * @brief Represents the best colouring for each strategy
     */
    struct ColouringResult {
        /**< The best configuration */
        Colouring s;
        /**< True if no correct colouring could be found for the particular configuration */
        bool isValid;
    };

    class ColouringAlgorithm {
    public:
        /**
         * The main entry point for executing the genetic algorithm in parallel.
         * It will maintain a population for each colouring strategy passed to this function.
         * For each iteration and strategy, the algorithm will choose randomly select one
         * operator to perform a corresponding action.
         * @param strategies the categories of operators and scoring functions used in this run
         * @param G the target graph
         * @param k the (maximum) number of colors
         * @param populationSize the number of maintained colourings
         * @param maxItr the maximum number of iterations
         * @param threadCount the number of used worker threads
         * @param outputStream if not null, it will be used to report recently found colourings
         * @return the best colourings for each passed colouring category
         */
        std::vector<ColouringResult> perform(const std::vector<std::unique_ptr<ColouringStrategy>> &strategies,
                                             const graph_access &G,
                                             ColorCount k,
                                             size_t populationSize,
                                             size_t maxItr,
                                             size_t threadCount = std::thread::hardware_concurrency(),
                                             std::ostream *outputStream = nullptr);
    };


}
