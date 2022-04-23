// #define __DEBUG_MODE // For testing purposes only
#include "B_DEQ.cpp"
#include "UNB_DEQ.cpp"
#include "WSQ.cpp"
#include <iostream>
#include <random>
#include <thread>
#include <vector>
using namespace std;

#define __DISABLE_STEALING

class WorkStealingThread {
  DEQueue **queue;
  int me;
  int totalQueues;

public:
  WorkStealingThread() {}
  WorkStealingThread(int me, DEQueue **queue, int n) {
    this->me = me;
    this->queue = queue;
    this->totalQueues = n;
    srand(0);
  }
  int get_thread_id() { return this->me; }
  void run() {
    printf("starting work steal %d\n", me);
    Runnable *task = queue[me]->popBottom();
    int count = 0;
    while (true) {
      while (task != nullptr) {
        printf("running\n");
        task->run();
        printf("ran\n");
        task = queue[me]->popBottom();
        count++;
      }

#ifndef __DISABLE_STEALING
      while (task == nullptr) {
        // std::this_thread::yield();
        sleep(0);
        int victim = 0;
        for (; victim < totalQueues; victim++) {
          if (victim == me || queue[victim]->isEmpty())
            continue;
          break;
        }
        if (victim + 1 >= totalQueues)
          return;
        if (!queue[victim]->isEmpty() && victim != me)
          task = queue[victim]->popTop();
        if (task != nullptr)
          printf("work stole %d --> %d\n", victim, me);
      }
#else
      return;
#endif
      sleep(0);
    }
  }
};

void add(int a, int b) { printf("testing and adding: %d\n", a + b); }

void thread_func(WorkStealingThread *wst) {
  wst->run();
  cout << "Exiting Thread_Func " << wst->get_thread_id() << endl;
}

void create_and_join_threads(int n, int k) {
  thread threads[n];
  DEQueue *queues[n];
  srand(time(0));

  for (int i = 0; i < n; i++) {
    // queues[i] = new BDEQueue(5);
    queues[i] = new BDEQueue(k);
    int len = k;
    for (int j = 0; j < len; j++) {
      auto a = Runnable(add, 1, 1);
      queues[i]->pushBottom(a);
    }
  }
  for (int i = 0; i < n; i++) {
    WorkStealingThread *wst = new WorkStealingThread(i, queues, n);
    threads[i] = thread(thread_func, wst);
  }

  for (int i = 0; i < n; i++) {
    threads[i].join();
  }
}

void a(Runnable *aa) { aa->run(); }

int main() {
  int n = 10, k = 1;
  // cin >> n >> k;
  create_and_join_threads(n, k);
  // b.popTop()->task();
}