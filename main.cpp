#include <utility>
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
  printf("Exiting Thread_Func %d \n",wst->get_thread_id());
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

long long fib(int n) {
  if(n <= 1) return n;
  return fib(n-1)+fib(n-2);
}
struct task_data{
  int n;
  long long res;

};
void add(task_data *result, int n) {
  result->res = fib(n);
  printf("testing and adding: %d ---> %lld\n", n, result->res);
}


task_data *arr;
int res_arr_count=0;
int main() {
  int n = 10, k = 1;
  cin >> n >> k;

  arr=new task_data[n*k];
  auto addRunnable = []() {
    int a = rand() % 10;
    arr[res_arr_count].n=a;
    return Runnable(add, &arr[res_arr_count++], a);
  };

  create_and_join_threads(n, k, addRunnable);

  for (int i=0;i<res_arr_count;i++){
    cout<<arr[i].n<<"\t"<<arr[i].res<<endl;
  }
}
