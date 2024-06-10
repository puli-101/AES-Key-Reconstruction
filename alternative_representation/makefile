FLAGS = -Iinclude

bin/heuristic: src/correctors/heuristic_bsc.cpp obj/aes_cpp.o
	mkdir -p bin/
	g++ -o $@ $^ $(FLAGS)

obj/aes_cpp.o: src/util/aes.c include/aes.h
	mkdir -p obj 
	g++ -g -c $< -o obj/aes_cpp.o $(FLAGS)

clean:
	rm -f obj/* bin/*