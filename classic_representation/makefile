FLAGS = -Iinclude -Wall -lm

OBJ = obj/aes.o obj/util.o obj/list.o

BINS = bin/erasures bin/keygen bin/noise

all: $(BINS)
.PRECIOUS: %.o

obj/%.o: src/util/%.c include/%.h
	mkdir -p obj
	gcc -g -c $< -o $@ $(FLAGS)

bin/%: src/%.c
	mkdir -p bin
	gcc -g -o $@ $^ $(FLAGS)

clean:
	rm -f obj/* bin/*

bin/%: src/correctors/%.c $(OBJ)
	mkdir -p bin
	gcc -g -o $@ $^ $(FLAGS)

bin/erasures: src/correctors/erasures.c $(OBJ)
	mkdir -p bin
	gcc -g -o $@ $^ $(FLAGS)

bin/%: src/generators/%.c $(OBJ)
	mkdir -p bin
	gcc -g -o $@ $^ $(FLAGS)
