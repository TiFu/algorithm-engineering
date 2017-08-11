#pragma once

#include "../graph_colouring.h"

namespace graph_colouring {
    /**
     * Naive implementation of the tabu search operator.
     * See Hybrid Evolutionary Algorithms for Graph Coloring, page 386
     * @param s the (invalid) colouring s of graph \p G
     * @param G the graph G
     * @param L the maximum number of iterations
     * @param A tuning parameter for table list length
     * @param alpha tuning parameter for table list length
     * @return an enhanced colouring based of configuration \p s
     */
    Colouring tabuSearchOperator(const Colouring &s,
                                 const graph_access &G,
                                 size_t L,
                                 size_t A,
                                 double alpha);
}