#include "colouring/ls/tabu_search.h"

configuration_t graph_colouring::tabuSearchOperator(const graph_access &G,
                                                    const configuration_t &s,
                                                    std::mt19937 &generator,
                                                    const size_t L,
                                                    const size_t A,
                                                    const double alpha) {
    //std::cerr << "MOP" << "\n";

    configuration_t s_mutated = graph_colouring::clone(s);
    std::uniform_int_distribution<size_t> distribution(0, A - 1);
    //tabu tenure
    const size_t tl = static_cast<size_t>(distribution(generator)
                                          + alpha * graph_colouring::numberOfConflictingNodes(G, s_mutated));
    //std::cerr << tl << "\n";


    //Number of classes
    const size_t k = s_mutated.size();
    //Number of nodes
    const size_t V = G.number_of_nodes();

    std::vector<size_t> tabu_table(V * k);
    for (size_t l = 0; l < L; l++) {
        //std::cerr << "l:" << l << "\n";
        //std::cerr << "l: " << l << "\n";
        NodeID best_v = std::numeric_limits<NodeID>::max();
        size_t best_i = std::numeric_limits<size_t>::max();
        size_t best_c_v = std::numeric_limits<size_t>::max();
        size_t best_score = graph_colouring::numberOfConflictingEdges(G, s_mutated);
        for (size_t c_v = 0; c_v < k; c_v++) {
            //std::cerr << "c_v:" << c_v << "\n";
            for (auto v : s_mutated[c_v]) {
                if (graph_colouring::isConflicting(v, s_mutated[c_v], G)) {
                    for (size_t i = 0; i < k; i++) {
                        if (i == c_v || tabu_table[v * k + c_v] > l) {
                            continue;
                        }
                        auto s_new = graph_colouring::clone(s_mutated);
                        s_new[c_v].erase(v);
                        s_new[i].insert(v);
                        auto new_score = graph_colouring::numberOfConflictingEdges(G, s_new);
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
        s_mutated[best_c_v].erase(best_v);
        s_mutated[best_i].insert(best_v);

        //std::cerr << "(v,i) = (" << best_v << "," << best_i << ") " << s_mutated << "\n";
        //std::cerr << tabu_table << "\n";
    }

    return s_mutated;
}