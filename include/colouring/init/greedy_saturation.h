#pragma once

#include "colouring/graph_colouring.h"

namespace graph_colouring {
    Configuration initByGreedySaturation(const graph_access &G,
                                           size_t k);
}
