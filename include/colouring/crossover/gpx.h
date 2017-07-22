#pragma once

#include "colouring/graph_colouring.h"

namespace graph_colouring {
    configuration_t gpxCrossover(const configuration_t &s1_org,
                                 const configuration_t &s2_org,
                                 std::mt19937 &generator);
}