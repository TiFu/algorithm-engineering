#pragma once

#include "colouring/graph_colouring.h"

namespace graph_colouring {
    configuration_t tabuSearchOperator(const graph_access &G,
                                       const configuration_t &s,
                                       const size_t L,
                                       const size_t A,
                                       const size_t alpha,
                                       std::mt19937 &generator);
}