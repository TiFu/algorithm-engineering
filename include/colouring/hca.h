#pragma once

#include "colouring/graph_colouring.h"

namespace graph_colouring {
    Configuration hybridColouringAlgorithm(const graph_access &G,
                                          size_t k,
                                          size_t population_size,
                                          size_t maxItr,
                                          size_t L,
                                          size_t A,
                                          double alpha);

}