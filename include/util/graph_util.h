#pragma once

#include "colouring/graph_colouring.h"

namespace graph_util {
    std::string toGraphvizStrig(const graph_access &G,
                                const std::string &label = "G");
    std::string toGraphvizStrig(const graph_access &G,
                                const configuration_t &configuration,
                                const std::string &label = "G");
}
