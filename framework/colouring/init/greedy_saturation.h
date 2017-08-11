#pragma once

#include "../graph_colouring.h"

namespace graph_colouring {
    /**
     * Naive implementation of the greedy saturation initialization operator.
     * See Hybrid Evolutionary Algorithms for Graph Coloring, page 385
     * @param k the number of used colors
     * @param G the target graph
     * @return a (possibly invalid) colouring with \p k colors
     */
    Colouring initByGreedySaturation(const graph_access &G,
                                     ColorCount k);
}
