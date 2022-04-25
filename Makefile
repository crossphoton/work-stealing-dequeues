build:
	mkdir ./bin
	g++ --std=c++17 ./benchmark/main.cpp -o ./bin/fibo.o

run: clean build
	mkdir -p ./logs
	./bin/fibo.o 100 100 unbounded > ./logs/unbounded_steal.log
	./bin/fibo.o 100 100 bounded > ./logs/bounded_steal.log
	./bin/fibo.o 100 100 unbounded --disableStealing > ./logs/unbounded_NoSteal.log
	./bin/fibo.o 100 100 bounded --disableStealing > ./logs/bounded_NoSteal.log
clean:
	rm -rf ./bin ./logs