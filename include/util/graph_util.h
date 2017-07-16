#pragma once

#include "data_structure/graph.h"

namespace graph_util {
    std::string toGraphvizStrig(const graph_access &G, const std::string label = "G");
}
