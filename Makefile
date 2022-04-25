main.out: main.o WSQ.o benchmarks/fibonacci.cpp
	g++ -pthread --std=c++17 WSQ.o main.o benchmarks/fibonacci.cpp -o main.out

WSQ.o: WSQ.cpp
	g++ -c WSQ.cpp

wsq: WSQ.o

main.o: main.cpp WSQ.h B_DEQ.cpp UNB_DEQ.cpp wsq_runner.cpp
	g++ -c main.cpp

run: main.out
	./main.out 1 100 unbounded 1 > unbounded_steal.log
	./main.out 1 100 bounded 1 > bounded_steal.log
	./main.out 1 100 unbounded 0 > unbounded_NoSteal.log
	./main.out 1 100 bounded 0 > bounded_NoSteal.log

clean:
	rm *.o *.out
