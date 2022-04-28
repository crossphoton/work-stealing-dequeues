# Work-Stealing-Dequeues

## Build

### Balanced Fibonacci Benchmark
```bash
$ make build app=fibonacci
```
### Skewed Fibonacci Benchmark
```
$ make build app=fibonacci_unbalanced
```
The above commands compiles the benchmark app and creates a ./bin/app.out
## Run
```
$ make run n=100 k=100
```
The above commands runs the benchmark with the following parameters: `n`: 100, `k`: 100.
n: is the number of threads.
k: is the number of tasks per threads.

This gives a console output of the configuration and the benchmark results.

A logs folder is created containing run logs of all the executions.

## Cleanup


```
$ make clean
```
or
```
$ @rm -rf ./bin ./logs
```
