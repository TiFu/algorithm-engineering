#include "colouring/crossover/gpx.h"

#include <algorithm>

inline Color biggestColorClass(const std::vector<size_t> &colorDist) {
    Color max = 0;
    for (Color c = 0; c < colorDist.size(); c++) {
        if (colorDist[c] > colorDist[max]) {
            max = c;
        }
    }
    return max;
}

graph_colouring::Colouring graph_colouring::gpxCrossover(const graph_colouring::Colouring &s1_org,
                                                               const graph_colouring::Colouring &s2_org) {
    assert(graph_colouring::colorCount(s1_org) == graph_colouring::colorCount(s2_org));

    Colouring s1(s1_org);
    Colouring s2(s2_org);

    size_t k = graph_colouring::colorCount(s1);

    Colouring s(s1.size(), std::numeric_limits<Color>::max());

    std::vector<size_t> colorDistS1(k);
    for (auto color: s1) {
        colorDistS1[color]++;
    }

    std::vector<size_t> colorDistS2(k);
    for (auto color: s2) {
        colorDistS2[color]++;
    }

    std::array<Colouring *, 2> V = {&s1, &s2};
    std::array<std::vector<size_t> *, 2> C = {&colorDistS1, &colorDistS2};

    for (Color l = 0; l < k; l++) {
        auto A = (l & 1);
        auto v = biggestColorClass(*C[A]);

        for (NodeID n = 0; n < s.size(); n++) {
            if ((*V[A])[n] == v) {
                s[n] = l;
                (*V[A])[n] = std::numeric_limits<Color>::max();
                (*C[A])[v]--;
                (*C[1 - A])[(*V[1 - A])[n]]--;
                (*V[1 - A])[n] = std::numeric_limits<Color>::max();
            }
        }
    }

    std::mt19937 generator;
    std::uniform_int_distribution<Color> distribution(0,
                                                      static_cast<Color>(colorDistS1.size() - 1));
    Color target = distribution(generator);

    for (auto &color : s) {
        if (color == std::numeric_limits<Color>::max()) {
            color = target;
        }
    }

    return s;
}

