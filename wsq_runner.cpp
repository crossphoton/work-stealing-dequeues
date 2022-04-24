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
  wst->run();
#ifdef __DEBUG_PRINT
  fprintf(stdout, "Exiting Thread_Func %d\n", wst->get_thread_id());
#endif
}

// Creates the work stealing thread with given runnable generator
void create_and_join_threads(int n, int k,
                             function<wsq::Runnable()> gen_runnable) {
  thread threads[n];
  wsq::DEQueue *queues[n];
  srand(time(0));

  for (int i = 0; i < n; i++) {
#ifdef WITH_BDEQUEUE
    queues[i] = new wsq::BDEQueue(k);
#else
    queues[i] = new wsq::UnboundedDEQueue(k);
#endif
    int len = rand() % k;
    for (int j = 0; j < len; j++)
      queues[i]->pushBottom(gen_runnable());
  }
  for (int i = 0; i < n; i++) {
    wsq::WorkStealingThread *wst = new wsq::WorkStealingThread(i, queues, n);
    threads[i] = thread(thread_func, wst);
  }

  for (int i = 0; i < n; i++) {
    threads[i].join();
  }
}
