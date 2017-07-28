#pragma once

#include "colouring/graph_colouring.h"

namespace graph_colouring {
    Configuration tabuSearchOperator(const graph_access &G,
                                       const Configuration &s,
                                       size_t L,
                                       size_t A,
                                       double alpha);
}