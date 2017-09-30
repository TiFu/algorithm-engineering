#include "benchmark/benchmark.h"

#include "data_structure/io/graph_io.h"
#include "util/graph_util.h"
#include "colouring/hca.h"
#include "colouring/init/xrlf.h"

using namespace graph_colouring;

auto BM_xrlf_optimal_colouring = [](benchmark::State &state, const char* _dc) {
    int iterations = 0;
    while (state.KeepRunning()) {
        graph_access G;
        std::string graphFile = "../../input/xrlf/" + std::to_string(state.range(0)) + "." + std::to_string(state.range(2)) + "." + std::to_string(state.range(1)) + ".graph";
        std::cout << "Graph File: " << graphFile << std::endl;
        graph_io::readGraphWeighted(G, graphFile);
        xrlf::Subgraph s(G);        
        Colouring c(G.number_of_nodes(), std::numeric_limits<NodeID>::max());
        Color offset = 0;
        auto start = std::chrono::high_resolution_clock::now();
        xrlf::findOptimalColouring(s, c, offset);
        auto end   = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(
        end - start);
        std::cout << "Elapse: " << elapsed_seconds.count() << std::endl;
        if (graph_colouring::numberOfConflictingNodes(G, c) > 0) {
            state.SkipWithError("Number of conflicting Nodes > 0!");
        }
        if (graph_colouring::numberOfConflictingEdges(G, c) > 0) {
            state.SkipWithError("Number of Conflicting Edges > 0!");            
        }
        state.SetIterationTime(elapsed_seconds.count());
        std::cout << "Done" << std::endl;
    }
};

auto BM_xrlf_independent_set = [](benchmark::State &state, const char* _dc) {
    int iterations = 0;
    while (state.KeepRunning()) {
        graph_access G;
        std::string graphFile = "../../input/xrlf/" + std::to_string(state.range(0)) + "." + std::to_string(state.range(2)) + "." + std::to_string(state.range(1)) + ".graph";
        std::cout << "Graph File: " << graphFile << std::endl;
        graph_io::readGraphWeighted(G, graphFile);
        xrlf::Subgraph s(G);        
        std::unordered_set<NodeID> C;
        xrlf::RandomAccessSet<NodeID> W(s.getNodes());

        auto start = std::chrono::high_resolution_clock::now();
        std::unordered_set<NodeID> is = exhaustiveSearch(W, s, C);
        auto end   = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(
        end - start);

        std::cout << "Elapse: " << elapsed_seconds.count() << std::endl;
        bool isIS = xrlf::isIndependentSet(is, G);
        if (!isIS) {
            state.SkipWithError("Number of conflicting Nodes > 0!");
        }

        state.SetIterationTime(elapsed_seconds.count());
        std::cout << "Done" << std::endl;
    }
};

auto BM_xrlf = [](benchmark::State &state,
            const char *graphFile) {
    int iterations = 0;
    while (state.KeepRunning()) {
        xrlf::XRLFParameters parameters;
        parameters.EXACTLIM = state.range(0);
        parameters.TRIALNUM = state.range(1);
        parameters.SETLIM = state.range(2);
        parameters.CANDNUM = state.range(3);
        parameters.MODE = xrlf::XRLFMode::IGNORE_COLORCOUNT;
        std::cout << graphFile << ": Exactlim: " << parameters.EXACTLIM << ", Trialnum: " << parameters.TRIALNUM << ", Setlim: " << parameters.SETLIM << ", Candnum: " << parameters.CANDNUM << std::endl; 
        
        graph_access G;
        graph_io::readGraphWeighted(G, graphFile);
        auto start = std::chrono::high_resolution_clock::now();
        Colouring c = xrlf::initByXRLF(G, parameters);
        auto end   = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(
        end - start);
        if (graph_colouring::numberOfConflictingNodes(G, c) > 0) {
            state.SkipWithError("Number of conflicting Nodes > 0!");
        }
        if (graph_colouring::numberOfConflictingEdges(G, c) > 0) {
            state.SkipWithError("Number of Conflicting Edges > 0!");            
        }
        state.SetIterationTime(elapsed_seconds.count());
        // This is a hack in order to report the number of used colors & the time & iteration
        state.counters["ITERATION_" + std::to_string(iterations)] = iterations;
        state.counters["TIME_" + std::to_string(iterations)] = elapsed_seconds.count();
        state.counters["COLORS_" + std::to_string(iterations)] = graph_colouring::colorCount(c);
        iterations++;
        std::cout << "Iteration Done" << std::endl;
    }
    state.counters["EXACTLIM"] = state.range(0);
    state.counters["TRIALNUM"] = state.range(1);
    state.counters["SETLIM"] = state.range(2);
    state.counters["CANDNUM"] = state.range(3);
};

static void XRLFArgumentsTENPERCENT(benchmark::internal::Benchmark* b) {
    int MAX_TRIALNUM = 1024; 
    int MAX_EXACTLIM = 70; 

    int setlim = 20;
    int candnum = 50;
    for (int trialnum = 32; trialnum <= MAX_TRIALNUM; trialnum *= 2) {
        for (int exactlim = 0; exactlim <= MAX_EXACTLIM; exactlim += 10) {
            b->Args({exactlim, trialnum, setlim, candnum});            
        }        
    }
}

static void XRLFArgumentsFIFTYPERCENT(benchmark::internal::Benchmark* b) {
    int MAX_TRIALNUM = 512; 
    int MAX_EXACTLIM = 70; 

    int setlim = 63;
    int candnum = 50;
    for (int trialnum = 32; trialnum <= MAX_TRIALNUM; trialnum *= 2) {
        for (int exactlim = 0; exactlim <= MAX_EXACTLIM; exactlim += 10) {
            b->Args({exactlim, trialnum, setlim, candnum});            
        }        
    }
}

static void XRLFArgumentsNINETYPERCENT(benchmark::internal::Benchmark* b) {
    int MAX_TRIALNUM = 128; 
    int MAX_EXACTLIM = 50; 

    int setlim = 63;
    int candnum = 50;
    for (int trialnum = 1; trialnum <= MAX_TRIALNUM; trialnum *= 2) {
        for (int exactlim = 0; exactlim <= MAX_EXACTLIM; exactlim += 10) {
            b->Args({exactlim, trialnum, setlim, candnum});            
        }        
    }
}


static void XRLFArguments250Exhaustive(benchmark::internal::Benchmark* b) {
    int MAX_TRIALNUM = 1;
    int MAX_EXACTLIM = 50;

    int setlim = 250;
    int candnum = 1;
    for (int exactlim = 0; exactlim <= MAX_EXACTLIM; exactlim += 10) {
        b->Args({exactlim, MAX_TRIALNUM, setlim, candnum});            
    }        
}

static void XRLFArguments500Exhaustive(benchmark::internal::Benchmark* b) {
    int MAX_TRIALNUM = 1;
    int MAX_EXACTLIM = 50;

    int setlim = 500;
    int candnum = 1;
    for (int exactlim = 0; exactlim <= MAX_EXACTLIM; exactlim += 10) {
        b->Args({exactlim, MAX_TRIALNUM, setlim, candnum});            
    }        
}

static void XRLFArguments1000Exhaustive(benchmark::internal::Benchmark* b) {
    int MAX_TRIALNUM = 1;
    int MAX_EXACTLIM = 50;

    int setlim = 1000;
    int candnum = 1;
    for (int exactlim = 0; exactlim <= MAX_EXACTLIM; exactlim += 10) {
        b->Args({exactlim, MAX_TRIALNUM, setlim, candnum});            
    }        
}

int REPETITIONS = 5;
// p = 0.1 

BENCHMARK_CAPTURE(BM_xrlf, DSJC250_TENPERCENT,
    "../../input/DSJC250.1-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArgumentsTENPERCENT)->Repetitions(REPETITIONS);

BENCHMARK_CAPTURE(BM_xrlf, DSJC500_TENPERCENT,
    "../../input/DSJC500.1-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArgumentsTENPERCENT)->Repetitions(REPETITIONS);

        
BENCHMARK_CAPTURE(BM_xrlf, DSJC1000_TENPERCENT,
    "../../input/DSJC1000.1-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArgumentsTENPERCENT)->Repetitions(REPETITIONS);

// p = 0.5

BENCHMARK_CAPTURE(BM_xrlf, DSJC250_FIFTYPERCENT,
    "../../input/DSJC250.5-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArgumentsFIFTYPERCENT)->Repetitions(REPETITIONS);

BENCHMARK_CAPTURE(BM_xrlf, DSJC500_FIFTYPERCENT,
    "../../input/DSJC500.5-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArgumentsFIFTYPERCENT)->Repetitions(REPETITIONS);

        
BENCHMARK_CAPTURE(BM_xrlf, DSJC1000_FIFTYPERCENT,
    "../../input/DSJC1000.5-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArgumentsFIFTYPERCENT)->Repetitions(REPETITIONS);


// p = 0.9

BENCHMARK_CAPTURE(BM_xrlf, DSJC250_NINETYPERCENT,
    "../../input/DSJC250.9-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArgumentsNINETYPERCENT)->Repetitions(REPETITIONS);

BENCHMARK_CAPTURE(BM_xrlf, DSJC500_NINETYPERCENT,
    "../../input/DSJC500.9-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArgumentsNINETYPERCENT)->Repetitions(REPETITIONS);

        
BENCHMARK_CAPTURE(BM_xrlf, DSJC1000_NINETYPERCENT,
    "../../input/DSJC1000.9-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArgumentsNINETYPERCENT)->Repetitions(REPETITIONS);
    
        
// exhaustive

BENCHMARK_CAPTURE(BM_xrlf, DSJC250_EXHAUSTIVE,
    "../../input/DSJC250.9-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArguments250Exhaustive)->Repetitions(REPETITIONS);

BENCHMARK_CAPTURE(BM_xrlf, DSJC500_EXHAUSTIVE,
    "../../input/DSJC500.9-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArguments500Exhaustive)->Repetitions(REPETITIONS);

BENCHMARK_CAPTURE(BM_xrlf, DSJC1000_EXHAUSTIVE,
    "../../input/DSJC1000.9-sorted.graph")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArguments1000Exhaustive)->Repetitions(REPETITIONS);

static void XRLFArgumentsOptimalColouring(benchmark::internal::Benchmark* b) {
    for (int percent = 1; percent <= 5; percent += 4) {
        for (int count = 1; count <= 10; count++) {
            for (int nodes = 10; nodes <= 80; nodes += 10) {
                b->Args({nodes, count, percent});            
            }
        }
    }        
};
        
BENCHMARK_CAPTURE(BM_xrlf_optimal_colouring, FIND_OPTIMAL_COLOURING, "")
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime()->Apply(XRLFArgumentsOptimalColouring)->Repetitions(REPETITIONS);

static void XRLFArgumentsIS(benchmark::internal::Benchmark* b) {
    for (int percent = 1; percent <= 5; percent += 4) {
        for (int nodes = 10; nodes <= 200; nodes += 10) {
            for (int count = 1; count <= 10; count++) {
                b->Args({nodes, count, percent});            
            }
        }        
    }
};
        
BENCHMARK_CAPTURE(BM_xrlf_independent_set, FIND_INDEPENDENT_SET, "")
->Unit(benchmark::kMillisecond)
->UseManualTime()->Apply(XRLFArgumentsIS)->Repetitions(REPETITIONS);
    
BENCHMARK_MAIN()
