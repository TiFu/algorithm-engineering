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