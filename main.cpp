#define __DEBUG_MODE // For testing purposes only
#define __DEBUG_PRINT // For testing purposes only
#define __DISABLE_STEALING // To disable stealing

#include "B_DEQ.cpp"
#include "UNB_DEQ.cpp"
#include "WSQ.cpp"
#include <functional>
#include <iostream>
#include <random>
#include <thread>
using namespace std;
using namespace wsq;

// Runs a WorkStealingThread
void thread_func(WorkStealingThread *wst) {
  wst->run();
  cout << "Exiting Thread_Func " << wst->get_thread_id() << endl;
}

// Creates the work stealing thread with given runnable generator
void create_and_join_threads(int n, int k, function<Runnable()> gen_runnable) {
  thread threads[n];
  DEQueue *queues[n];
  srand(time(0));

  for (int i = 0; i < n; i++) {
    // queues[i] = new BDEQueue(5);
    queues[i] = new BDEQueue(k);
    int len = rand() % k;
    for (int j = 0; j < len; j++)
      queues[i]->pushBottom(gen_runnable());
  }
  for (int i = 0; i < n; i++) {
    WorkStealingThread *wst = new WorkStealingThread(i, queues, n);
    threads[i] = thread(thread_func, wst);
  }

  for (int i = 0; i < n; i++) {
    threads[i].join();
  }
}

void add(int a, int b) {
  sleep(1);
  printf("testing and adding: %d\n", a + b);
}

int main() {
  int n = 10, k = 1;
  cin >> n >> k;

  auto addRunnable = []() {
    int a = rand() % 100, b = rand() % 100;
    return Runnable(add, a, b);
  };

  create_and_join_threads(n, k, addRunnable);
}
