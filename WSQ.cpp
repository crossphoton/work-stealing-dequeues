#ifndef _PCP_WS_QUEUE
#define _PCP_WS_QUEUE

#include "WSQ.h"
#include <atomic>
#include <functional>
#include <thread>
#include <unistd.h>

#ifdef __DEBUG_PRINT
#include <stdio.h>
#endif

namespace wsq {

template <typename T>
AtomicStampedReference<T>::AtomicStampedReference(T data) {
  snap.store(AtomicStamp<T>(data));
}

template <typename T>
AtomicStampedReference<T>::AtomicStampedReference(T data, int stamp) {
  snap.store(AtomicStamp<T>(data, stamp));
}

template <typename T>
bool AtomicStampedReference<T>::compareAndSet(T oldRef, T newRef, int oldStamp,
                                              int newStamp) {
  AtomicStamp<T> old = this->snap.load();
  AtomicStamp<T> newVal = AtomicStamp<T>(newRef, newStamp);
  return snap.compare_exchange_strong(old, newVal, std::memory_order_release,
                                      std::memory_order_relaxed);
}

template <typename T> T AtomicStampedReference<T>::get(int *ptr) {
  AtomicStamp<T> temp = snap.load();

  ptr[0] = temp.timestamp;
  return temp.value;
}

template <typename T> T AtomicStampedReference<T>::getReference() {
  return snap.load().value;
}

template <typename T> int AtomicStampedReference<T>::getStamp() {
  return snap.load().timestamp;
}

template <typename T> void AtomicStampedReference<T>::set(T data) {
  AtomicStamp<T> temp = AtomicStamp<T>(data);
  snap.store(temp);
}

template <typename T> void AtomicStampedReference<T>::set(T data, int stamp) {
  AtomicStamp<T> temp = AtomicStamp<T>(data, stamp);
  snap.store(temp);
}

long Runnable::__runnable_id = 0;

WorkStealingThread::WorkStealingThread(int me, DEQueue **queue, int n) {
  this->me = me;
  this->queue = queue;
  this->totalQueues = n;
  srand(0);
}

int WorkStealingThread::get_thread_id() { return this->me; }

void WorkStealingThread::run() {
#ifdef __DEBUG_PRINT
  printf("starting work steal %d\n", me);
#endif
  Runnable *task = queue[me]->popBottom();
  int count = 0;
  while (true) {
    while (task != nullptr) {
      task->run();
      task = queue[me]->popBottom();
      count++;
    }

#ifndef __DISABLE_STEALING
    while (task == nullptr) {
      std::this_thread::yield();
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

#ifdef __DEBUG_PRINT
      if (task != nullptr)
        printf("work stole %d --> %d\n", victim, me);
#endif
    }
#else
    return;
#endif
    std::this_thread::yield();
  }
}
} // namespace wsq

#endif
