#define __DEBUG_PRINT
#include <cstdio>

#include "WSQ.h"
#include "task.h"
#include "wsq_runner.cpp"
#include <stdlib.h>
#include <string.h>

long getEpoch()
{
    using namespace std::chrono;
    microseconds ms = duration_cast<microseconds>(
        system_clock::now().time_since_epoch());

    return ms.count();
}

int main(int argc, char **argv) {
  srand(time(0));
  if (argc < 5) {
    fprintf(stderr,
            "not enough command arguments provided.\nrequired = 4 ; provided = "
            "%d\n",
            argc);
    return 1;
  }
  int n = atoi(argv[1]), k = atoi(argv[2]);
  bool bounded = strcmp(argv[3],"bounded");
  bool enableStealing = atoi(argv[4]) == 1;

  init(n, k);
  
  auto start = getEpoch();
  create_and_join_threads(n, k, runnable_generator, bounded, enableStealing);
  auto end = getEpoch();

  collect_results(end-start, start, end);
}
