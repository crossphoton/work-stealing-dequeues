build: clean
	@mkdir ./bin
	@g++ -pthread --std=c++17 ./benchmark/$(application).cpp -o ./bin/app.out

run:
	@mkdir -p ./logs

	@./bin/app.out  $(n) $(k) bounded > ./logs/bounded_steal.log
	@head -1 ./logs/bounded_steal.log
	@cat ./logs/bounded_steal.log | grep -c "work stole"
	@tail -2 ./logs/bounded_steal.log
	@echo "----------------------"

	@./bin/app.out  $(n) $(k) bounded --disableStealing > ./logs/bounded_NoSteal.log
	@head -1 ./logs/bounded_NoSteal.log
	@tail -2 ./logs/bounded_NoSteal.log
	@echo "----------------------"
	
	@./bin/app.out  $(n) $(k) unbounded > ./logs/unbounded_steal.log

	@head -1 ./logs/unbounded_steal.log
	@cat ./logs/unbounded_steal.log | grep -c "work stole"
	@tail -2 ./logs/unbounded_steal.log
	@echo "----------------------"



	@./bin/app.out  $(n) $(k) unbounded --disableStealing > ./logs/unbounded_NoSteal.log
	@head -1 ./logs/unbounded_NoSteal.log
	@tail -2 ./logs/unbounded_NoSteal.log
	@echo "----------------------"



clean:
	@rm -rf ./bin ./logs