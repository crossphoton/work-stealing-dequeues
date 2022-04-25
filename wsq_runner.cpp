#include "B_DEQ.cpp"
#include "UNB_DEQ.cpp"
#include "WSQ.h"
#include <functional>
#include <iostream>
#include <random>
#include <thread>
using namespace std;

// Runs a WorkStealingThread
void thread_func(wsq::WorkStealingThread *wst) {
#ifdef __DEBUG_PRINT
  fprintf(stdout, "Entering Thread_Func %d\n", wst->get_thread_id());
#endif
  wst->run();
#ifdef __DEBUG_PRINT
  fprintf(stdout, "Exiting Thread_Func %d\n", wst->get_thread_id());
#endif
}

// Creates the work stealing thread with given runnable generator
void create_and_join_threads(int n, int k,
                             function<wsq::Runnable()> gen_runnable,
                             bool isBounded, bool enableStealing) {
  thread threads[n];
  wsq::DEQueue *queues[n];
  srand(time(0));

  printf("created for thread %d\n", 4);
  for (int i = 0; i < n; i++) {

    if (isBounded)
      queues[i] = new wsq::BDEQueue(k);
    else
      queues[i] = new wsq::UnboundedDEQueue(k);
    for (int j = 0; j < k; j++)
      queues[i]->pushBottom(gen_runnable());
  }
  printf("stealing enabled %d\n", enableStealing);
  for (int i = 0; i < n; i++) {
    wsq::WorkStealingThread *wst =
        new wsq::WorkStealingThread(i, queues, n, enableStealing);
    threads[i] = thread(thread_func, wst);
  }

  for (int i = 0; i < n; i++) {
    threads[i].join();
  }

  // for (int i = 0; i < n; i++) {
  //   delete queues[i];
  // }
}