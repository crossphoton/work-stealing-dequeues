#include <utility>
#define __DEBUG_MODE       // For testing purposes only
#define __DEBUG_PRINT      // For testing purposes only
#define __DISABLE_STEALING // To disable stealing
#include <iostream>
#include <random>
#include <cstring>
#include "./utills.cpp"
using namespace std;
using namespace wsq;

/**
 * "If n is less than or equal to 1, return n. Otherwise, return the sum of the previous two Fibonacci
 * numbers."
 *
 * @param n The number of fibonacci numbers to generate.
 *
 * @return The nth number in the Fibonacci sequence.
 */
long long fib(int n)
{
  if (n <= 1)
    return n;
  return fib(n - 1) + fib(n - 2);
}
struct task_data
{
  int n;
  long long res;
  long long createdAt;
  long long completedAt;
};
void fib_thread(task_data *result, int n)
{
  result->res = fib(n);
}

task_data *arr;
int res_arr_count = 0;

int main(int argc, char **argv)
{
  /* Used to seed the random number generator. */
  srand(time(0));
  /* Parsing the command line arguments. */
  int n = atoi(argv[1]), k = atoi(argv[2]);
  bool isBounded;
  if (strcmp(argv[3], "bounded") == 0)
    isBounded = true;
  else if (strcmp(argv[3], "unbounded") == 0)
    isBounded = false;
  else
  {
    fprintf(stderr, "invalid bounded/unbounded argument\n");
    return 1;
  }
  bool enableStealing = true;
  if (argv[4] != NULL)
  {
    if (strcmp(argv[4], "--disableStealing") == 0)
    {
      enableStealing = false;
    }
  }
  cout << "n: " << n << " k: " << k << " bounded: " << isBounded << " enableStealing: " << enableStealing << endl;

  /* Creating an array of task_data structs. */
  arr = new task_data[n * k];
  /* A lambda function which is used to generate a runnable. */
  auto addRunnable = []()
  {
    int a = rand() % 25;
    arr[res_arr_count].n = a;
    return Runnable(fib_thread, &arr[res_arr_count++], a);
  };

  /* Calculating the time taken to execute the program. */
  auto start = getEpoch();
  create_and_join_threads(n, k, addRunnable, isBounded, enableStealing);
  auto end = getEpoch();
  cout << "Time taken: " << end - start << " us" << endl;

  // for (int i=0;i<res_arr_count;i++){
  //   cout<<arr[i].n<<"\t"<<arr[i].res<<endl;
  // }
}
