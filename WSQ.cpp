#ifndef _PCP_WS_QUEUE
#define _PCP_WS_QUEUE

#include <atomic>
#include <functional>
#include <stdio.h>
#include <thread>
#include <unistd.h>

namespace wsq {

template <typename T> struct AtomicStamp {
  T value;
  int timestamp;
  AtomicStamp(T data) noexcept : value(data), timestamp(0) {}
  AtomicStamp() noexcept : timestamp(0) {}
  AtomicStamp(T data, int stamp) noexcept : value(data), timestamp(stamp) {}
  bool operator==(const AtomicStamp<T> &other) const {
    return value == other.value && timestamp == other.timestamp;
  }
};

template <typename T> class AtomicStampedReference {

public:
  std::atomic<AtomicStamp<T>> snap;
  AtomicStampedReference() {}
  AtomicStampedReference(T data) { snap.store(AtomicStamp<T>(data)); }
  AtomicStampedReference(T data, int stamp) {
    snap.store(AtomicStamp<T>(data, stamp));
  }

  bool compareAndSet(T oldRef, T newRef, int oldStamp, int newStamp) {
    AtomicStamp<T> old = this->snap.load();
    AtomicStamp<T> newVal = AtomicStamp<T>(newRef, newStamp);
    return snap.compare_exchange_strong(old, newVal, std::memory_order_release,
                                        std::memory_order_relaxed);
  }

  T get(int *ptr) {
    AtomicStamp<T> temp = snap.load();

    ptr[0] = temp.timestamp;
    return temp.value;
  }
  T getReference() { return snap.load().value; }
  int getStamp() { return snap.load().timestamp; }
  void set(T data) {
    AtomicStamp<T> temp = AtomicStamp<T>(data);
    snap.store(temp);
  }
  void set(T data, int stamp) {
    AtomicStamp<T> temp = AtomicStamp<T>(data, stamp);
    snap.store(temp);
  }
};

long __runnable_id = 0;
class Runnable {
  long _gen_id() { return ++__runnable_id; }
  long id;

public:
  std::function<void()> task = NULL;
  template <typename func, typename... Args> Runnable(func f, Args... args) {
    id = _gen_id();
    this->task = [=]() { return f(args...); };
  }
  void run() {
    if (task != NULL)
      task();
  }
  Runnable() {}
};

class DEQueue {
public:
  Runnable *tasks;
  volatile int bottom;
  AtomicStampedReference<int> *top;
  virtual void pushBottom(Runnable r) = 0;
  virtual bool isEmpty() = 0;
  virtual Runnable *popTop() = 0;
  virtual Runnable *popBottom() = 0;
};

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
      sleep(0);
    }
  }
};
} // namespace wsq

#endif
