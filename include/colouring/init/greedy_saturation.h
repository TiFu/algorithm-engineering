#pragma once

#include "colouring/graph_colouring.h"

namespace graph_colouring {
    configuration_t initByGreedySaturation(const graph_access &G,
                                           size_t k,
                                           std::mt19937 &generator);
}
