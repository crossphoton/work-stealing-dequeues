#include <functional>
#include <iostream>
#include <stdio.h>

void add(int a, int b, int c) { printf("addition is : %d\n", a + b + c); }



class Runnable {
public:
  std::function<void()> run;
  template <typename func, typename... Args> Runnable(func f, Args... args) {
    this->run = [&f, &args...]() { return f(args...); };
  }
  Runnable() {}
};

Runnable *b;

void run() {
	b[0].run();
}

void store (Runnable a) {
	b = new Runnable[20];
	b[0] = a;
	// b[0].run();
}

int main() {

  auto a = Runnable(add, 2, 4, 9);
  store(a);
	run();
}