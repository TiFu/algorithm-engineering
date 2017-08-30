#pragma once

#include "graph_colouring.h"

namespace graph_colouring {
    /**
     * Very naive implementation of the hybrid coloring algorithm.
     * See Hybrid Evolutionary Algorithms for Graph Coloring.
     * @param G the target graph
     * @param k the (maximum) number colors allowed for colouring
     * @param populationSize the number of maintained colourings for the graph \p G
     * @param maxItr the maximum number of iterations
     * @param L number of iterations for the tabu search operator
     * @param A tuning parameter for tabu search operator
     * @param alpha tuning parameter for tabu search operator
     * @param repeat if true, the algorithm will repeat the entire execution
     * with a smaller value for k (k-1, k-2, k-3, ...) as long as a valid solution
     * for k-i coloring could be obtained in the previous runs
     * @param logStream if specified, the algorithm will print out the results of each
     * iteration into the output stream
     * @param outputStream if not null, it will be used to report recently found colourings
     * @return the best found colouring
     */
    ColouringResult hybridColouringAlgorithm(const graph_access &G,
                                             ColorCount k,
                                             size_t populationSize,
                                             size_t maxItr,
                                             size_t L,
                                             size_t A,
                                             double alpha,
                                             size_t threadCount = std::thread::hardware_concurrency(),
                                             std::ostream *outputStream = nullptr);

}