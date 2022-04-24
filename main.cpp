#include <cstdio>
#define WITH_BDEQUEUE       // If not defined unbounded queue is used

#include "WSQ.h"
#include "wsq_runner.cpp"
// #include "task.h"
#include "benchmarks/fibonacci.cpp"
#include <stdlib.h>

int main(int argc, char **argv) {
  if(argc < 3)
    fprintf(stderr, "not enough command arguments provided.\nrequired = 2 ; provided = %d\n", argc);
  int n = atoi(argv[1]), k = atoi(argv[2]);

  create_and_join_threads(n, k, runnable_generator);

  collect_results();
}
