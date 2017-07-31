#pragma once

#include "colouring/graph_colouring.h"

namespace graph_colouring {
    /**
     * Naive implementation of the greedy saturation initialization operator.
     * See Hybrid Evolutionary Algorithms for Graph Coloring, page 385
     * @param G the target graph
     * @param k the number of used colors
     * @return a (possibly invalid) colouring with \p k colors
     */
    Configuration initByGreedySaturation(const graph_access &G,
                                         size_t k);
}
