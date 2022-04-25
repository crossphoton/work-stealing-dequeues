build:
	g++ --std=c++17 main.cpp -o main.out

run: build
	mkdir -p ./logs
	./main.out 100 100 unbounded > ./logs/unbounded_steal.log
	./main.out 100 100 bounded > ./logs/bounded_steal.log
	./main.out 100 100 unbounded --disableStealing > ./logs/unbounded_NoSteal.log
	./main.out 100 100 bounded --disableStealing > ./logs/bounded_NoSteal.log