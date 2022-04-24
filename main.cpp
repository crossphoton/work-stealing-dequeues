// #define __DEBUG_PRINT
#define __DISABLE_STEALING
#include <cstdio>
#define WITH_BDEQUEUE // If not defined unbounded queue is used

#include "WSQ.h"
#include "task.h"
#include "wsq_runner.cpp"
#include <stdlib.h>

long getEpoch()
{
    using namespace std::chrono;
    microseconds ms = duration_cast<microseconds>(
        system_clock::now().time_since_epoch());

    return ms.count();
}

int main(int argc, char **argv) {
  srand(time(0));
  if (argc < 3) {
    fprintf(stderr,
            "not enough command arguments provided.\nrequired = 2 ; provided = "
            "%d\n",
            argc);
    return 1;
  }
  int n = atoi(argv[1]), k = atoi(argv[2]);
  
  auto start = getEpoch();
  create_and_join_threads(n, k, runnable_generator);
  auto end = getEpoch();

  collect_results(end-start, start, end);
}
