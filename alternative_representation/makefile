FLAGS = -Iinclude -O2

all: bin/heuristic bin/bruteforce bin/keygen

bin/heuristic: src/correctors/heuristic_bsc.cpp obj/aes_cpp.o obj/util_cpp.o obj/resolution_handler_cpp.o
	mkdir -p bin/
	g++ -o $@ $^ $(FLAGS)

obj/%_cpp.o: src/util/%.c include/%.h
	mkdir -p obj 
	g++ -g -c $< -o $@ $(FLAGS) 

bin/bruteforce: src/correctors/bruteforce_bsc.c obj/aes.o obj/util.o obj/resolution_handler.o obj/candidate_handler.o
	mkdir -p bin/
	gcc -o $@ $^ $(FLAGS) -lm

bin/keygen: src/generators/keygen.c obj/aes.o obj/util.o
	mkdir -p bin/
	gcc -o $@ $^ $(FLAGS)

obj/%.o: src/util/%.c include/%.h
	mkdir -p obj
	gcc -g -c $< -o $@ $(FLAGS)

clean:
	rm -f obj/* bin/*