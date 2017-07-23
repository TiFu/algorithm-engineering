#pragma once

#include "colouring/graph_colouring.h"

namespace graph_colouring {
    configuration_t tabuSearchOperator(const graph_access &G,
                                       const configuration_t &s,
                                       std::mt19937 &generator,
                                       size_t L,
                                       size_t A,
                                       double alpha);
}