# Writeup
**Please do not use the (very small) graphs in the ```input``` directory for your evaluation - it is only there to show you how to use the graph data structure. Use the instances mentioned below.**

You have to submit a writeup of your task. You should follow this structure:
* Abstract 
* Introduction 
* Implementation (Data structures, algorithms, maybe time and space complexity)
* Experimental Evaluation
    * Experimental Setup (What exactly do you do in your experiments, what parameters do you chose, which graphs do you use)
    * Experiments (e.g. tables presenting your results, plots with quality over time. Carefully chose how to present your results - for example appropriate averaging, normalizing...)
        * Interpretation of your results(!)

# Build:

```sh
$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ../
$ make
```

File [tests/main.cpp] [tests/main.cpp] contains an example how to use graph data structure.

```sh
$ build/tests/test_graph input/cti.graph
```
[tests/main.cpp]: <https://git.scc.kit.edu/ITI10/algorithm_engineering2017/blob/master/tests/main.cpp>


# Graphs for coloring:
You can download graphs for coloring from [here](http://algo2.iti.kit.edu/akhremtsev/coloring_graphs/coloring_graphs.tar).
So far these are the graphs from the DIMACS coloring chalenge. You can find descriptions of graphs [here](http://mat.gsia.cmu.edu/COLOR/instances.html).


# Benchmarking
Please create your own branch with the name <your_surname>.
You can write scripts that can be executed on the benchmark server in [test.sh](test.sh). Every file that is in ```results/``` after your script will be downloadable later - so output your timings there! To run your benchmarks and get your results, click on "Pipelines". Click on the arrow next to your commit and select "benchmark". Once your run finishes, it will say "passed". Click on "passed" -> "benchmark". Here you can see the output of your script. On the right hand side there is a link "Download" where you can download an archive with your files.
You can find the graphs for benchmarking in ```$HOME/graphs/coloring_graphs```. You can iterate over the files in this directory and use them as input to your algorithm.
