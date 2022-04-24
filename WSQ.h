#ifndef _PCP_WS_QUEUE_H
#define _PCP_WS_QUEUE_H

#include <atomic>
#include <functional>
#include <thread>
#include <unistd.h>

#ifdef __DEBUG_PRINT
#include <stdio.h>
#endif

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
  AtomicStampedReference(T data);
  AtomicStampedReference(T data, int stamp);

  bool compareAndSet(T oldRef, T newRef, int oldStamp, int newStamp);
  T get(int *ptr);
  T getReference();
  int getStamp();
  void set(T data);
  void set(T data, int stamp);
};

class Runnable {
  long _gen_id() { return ++__runnable_id; }
  long id;
  static long __runnable_id;

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
  WorkStealingThread(int me, DEQueue **queue, int n);
  int get_thread_id();
  void run();
};
} // namespace wsq

#endif
