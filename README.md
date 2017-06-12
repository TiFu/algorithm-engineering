To build the project use following command:

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
