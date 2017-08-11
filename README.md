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