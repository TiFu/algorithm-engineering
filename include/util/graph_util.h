#pragma once

#include "colouring/graph_colouring.h"

namespace graph_util {
    /**
     * @param G the target graph
     * @param label a label used for the graph visualization

     * @return a textual representation of the given graph \p G in GraphViz format
     */
    std::string toGraphvizStrig(const graph_access &G,
                                const std::string &label = "G");

    /**
     * @param G the target graph
     * @param configuration the colouring of the graph \p G
     * @param partitioned if true, the output string will represent a GraphViz graph that is partitioned base don the colouring
     * @param label a label used for the graph visualization
     * @return a textual representation of the given graph \p G with its colouring \p configuration in GraphViz format
     */
    std::string toGraphvizStrig(const graph_access &G,
                                const graph_colouring::Configuration &configuration,
                                bool partitioned = false,
                                const std::string &label = "G");
}
