main.out: main.cpp WSQ.cpp B_DEQ.cpp UNB_DEQ.cpp
	g++ -pthread --std=c++17 main.cpp -o main.out

run: main.out
	./main.out