#include <utility>
#define __DEBUG_MODE // For testing purposes only
#define __DEBUG_PRINT // For testing purposes only
#define __DISABLE_STEALING // To disable stealing

#include "./../src/B_DEQ.cpp"
#include "./../src/UNB_DEQ.cpp"
#include "./../src/WSQ.cpp"
#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include <cstring>
using namespace std;
using namespace wsq;

// Runs a WorkStealingThread
void thread_func(WorkStealingThread *wst) {
  wst->run();
  printf("Exiting Thread_Func %d \n",wst->get_thread_id());
}

// Creates the work stealing thread with given runnable generator
void create_and_join_threads(int n, int k, function<Runnable()> gen_runnable,bool isBounded, bool enableStealing) {
  thread threads[n];
  DEQueue *queues[n];
  srand(time(0));

  for (int i = 0; i < n; i++) {
    // queues[i] = new BDEQueue(5);
    if(isBounded)
      queues[i] = new BDEQueue(k);
    else
      queues[i] = new UnboundedDEQueue(k);
    int len = rand() % k;
    for (int j = 0; j < len; j++)
      queues[i]->pushBottom(gen_runnable());
  }
  for (int i = 0; i < n; i++) {
    WorkStealingThread *wst = new WorkStealingThread(i, queues, n, enableStealing);
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
void fib_thread(task_data *result, int n) {
  result->res = fib(n);
}

long getEpoch()
{
    using namespace std::chrono;
    microseconds ms = duration_cast<microseconds>(
        system_clock::now().time_since_epoch());

    return ms.count();
}

task_data *arr;
int res_arr_count=0;
int main(int argc, char **argv) {
  srand(time(0));
  int n = atoi(argv[1]), k = atoi(argv[2]);
  bool isBounded;
  if (strcmp(argv[3], "bounded") == 0)
    isBounded = true;
  else if (strcmp(argv[3], "unbounded") == 0)
    isBounded = false;
  else {
    fprintf(stderr, "invalid bounded/unbounded argument\n");
    return 1;
  }
  bool enableStealing=true;
  if(argv[4] != NULL)
  {
    if(strcmp(argv[4],"--disableStealing") == 0)
    {
      enableStealing = false;   
    }
  }
  cout << "n: " << n << " k: " << k << " bounded: " << isBounded << " enableStealing: " << enableStealing << endl;

  arr=new task_data[n*k];
  auto addRunnable = []() {
    int a = rand() % 25;
    arr[res_arr_count].n=a;
    return Runnable(fib_thread, &arr[res_arr_count++], a);
  };
  
  auto start = getEpoch();
  create_and_join_threads(n, k, addRunnable, isBounded, enableStealing);
  auto end = getEpoch();

  cout <<"Time taken: " << end - start << " us" << endl;

  // for (int i=0;i<res_arr_count;i++){
  //   cout<<arr[i].n<<"\t"<<arr[i].res<<endl;
  // }
}
