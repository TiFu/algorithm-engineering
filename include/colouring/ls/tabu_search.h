#pragma once

#include "colouring/graph_colouring.h"

namespace graph_colouring {
    /**
     * Naive implementation of the tabu search operator.
     * See Hybrid Evolutionary Algorithms for Graph Coloring, page 386
     * @param G the graph G
     * @param s the (invalid) colouring s of graph \p G
     * @param L the maximum number of iterations
     * @param A tuning parameter for table list length
     * @param alpha tuning parameter for table list length
     * @return an enhanced colouring based of configuration \p s
     */
    Colouring tabuSearchOperator(const graph_access &G,
                                     const Colouring &s,
                                     size_t L,
                                     size_t A,
                                     double alpha);
}