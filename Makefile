main.out: main.o wsq
	g++ -pthread --std=c++17 WSQ.o main.o benchmarks/fibonacci.cpp -o main.out

run: main.out
	./main.out

WSQ.o: WSQ.cpp
	g++ -c WSQ.cpp

wsq: WSQ.o

main.o: main.cpp WSQ.h B_DEQ.cpp UNB_DEQ.cpp
	g++ -c main.cpp

run: main.out
	./main.out 5 5