#include "colouring/crossover/gpx.h"

#include <algorithm>
#include <array>

configuration_t graph_colouring::gpxCrossover(const configuration_t &s1_org,
                                              const configuration_t &s2_org,
                                              std::mt19937 &generator) {
    assert(s1_org.size() == s2_org.size());

    auto s1 = graph_colouring::clone(s1_org);
    auto s2 = graph_colouring::clone(s2_org);

    configuration_t s;
    s.resize(s1.size());

    std::array<configuration_t *, 2> V = {&s1, &s2};

    for (size_t l = 0; l < s1.size(); l++) {
        auto A = (l & 1);
        auto v = std::max_element(V[A]->begin(),
                                  V[A]->end(),
                                  [](const partition_t &a, const partition_t &b) {
                                      return a.size() < b.size();
                                  });
        s[l].insert(v->begin(), v->end());
        v->clear();
        for (auto n : s[l]) {
            for (auto &p : *V[1 - A]) {
                p.erase(n);
            }
        }
    }
    partition_t rest;
    for (auto &p : *V[0]) {
        rest.insert(p.begin(), p.end());
    }
    std::uniform_int_distribution<size_t> distribution(0, s.size() - 1);
    s[distribution(generator)].insert(rest.begin(), rest.end());

    return s;
}

