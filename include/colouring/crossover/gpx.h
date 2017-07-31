#pragma once

#include "colouring/graph_colouring.h"

namespace graph_colouring {
    /**
     * Naive implementation of the Greedy Partition Crossover (GPX) Operator.
     * See Hybrid Evolutionary Algorithms for Graph Coloring, page 385
     * @param s1 the first parent
     * @param s2 the second parent
     * @return a new colouring based on the two parents
     */
    Configuration gpxCrossover(const Configuration &s1,
                                 const Configuration &s2);
}