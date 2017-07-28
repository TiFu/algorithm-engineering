#include "colouring/ls/tabu_search.h"

configuration_t graph_colouring::tabuSearchOperator(const graph_access &G,
                                                    const configuration_t &s,
                                                    const size_t L,
                                                    const size_t A,
                                                    const double alpha) {
    configuration_t s_mutated(s);
    std::uniform_int_distribution<size_t> distribution(0, A - 1);
    std::mt19937 generator;
    //tabu tenure
    auto tl = static_cast<size_t>(distribution(generator)
                                          + alpha * graph_colouring::numberOfConflictingNodes(G, s_mutated));


    //Number of classes
    const size_t k = graph_colouring::colorCount(s_mutated);
    //Number of nodes
    const size_t V = G.number_of_nodes();

    std::vector<size_t> tabu_table(V * k);
    for (size_t l = 0; l < L; l++) {
        NodeID best_v = std::numeric_limits<NodeID>::max();
        Color best_i = std::numeric_limits<Color>::max();
        Color best_c_v = std::numeric_limits<Color>::max();
        size_t best_score = graph_colouring::numberOfConflictingEdges(G, s_mutated);
        for (Color c_v = 0; c_v < k; c_v++) {
            for (NodeID v = 0; v < G.number_of_nodes(); v++) {
                if (s_mutated[v] == c_v && !graph_colouring::allowedInClass(G, s_mutated, c_v, v)) {
                    for (Color i = 0; i < k; i++) {
                        if (i == c_v || tabu_table[v * k + c_v] > l) {
                            continue;
                        }
                        s_mutated[v] = i;
                        auto new_score = graph_colouring::numberOfConflictingEdges(G, s_mutated);
                        s_mutated[v] = c_v;
                        if (new_score <= best_score) {
                            best_v = v;
                            best_i = i;
                            best_c_v = c_v;
                            best_score = new_score;
                        }
                    }
                }
            }
        }
        //nothing to improve
        if (best_v == std::numeric_limits<NodeID>::max()) {
            break;
        }
        tabu_table[best_v * k + best_c_v] = ((l + 1) + tl);
        s_mutated[best_v] = best_i;
    }

    return s_mutated;
}