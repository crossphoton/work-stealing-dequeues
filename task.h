#ifndef _TASK_DEF
#define _TASK_DEF

#include "WSQ.h"

void init(int, int);
wsq::Runnable runnable_generator();
void collect_results(long, long, long);

#endif