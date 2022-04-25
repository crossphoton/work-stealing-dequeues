build:
	@mkdir ./bin
	@g++ --std=c++17 ./benchmark/main.cpp -o ./bin/fibo.o

run: clean build
	@mkdir -p ./logs
	
	@./bin/fibo.o 100 100 unbounded > ./logs/unbounded_steal.log
	@echo "----------------------"
	@head -1 ./logs/unbounded_steal.log
	@cat ./logs/unbounded_steal.log | grep -c "work stole"
	@tail -1 ./logs/unbounded_steal.log

	@./bin/fibo.o 100 100 bounded > ./logs/bounded_steal.log
	@echo "----------------------"
	@head -1 ./logs/bounded_steal.log
	@cat ./logs/bounded_steal.log | grep -c "work stole"
	@tail -1 ./logs/bounded_steal.log

	@./bin/fibo.o 100 100 unbounded --disableStealing > ./logs/unbounded_NoSteal.log
	@echo "----------------------"
	@head -1 ./logs/unbounded_NoSteal.log
	@tail -1 ./logs/unbounded_NoSteal.log

	@./bin/fibo.o 100 100 bounded --disableStealing > ./logs/bounded_NoSteal.log
	@echo "----------------------"
	@head -1 ./logs/bounded_NoSteal.log
	@tail -1 ./logs/bounded_NoSteal.log

clean:
	rm -rf ./bin ./logs