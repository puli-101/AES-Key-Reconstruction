FLAGS = -Iinclude -Wall -lm

OBJ = obj/aes.o obj/util.o obj/list.o obj/candidate_handler.o

CORRECTORS = bin/bruteforce_alt_bsc bin/erasures bin/z_channel
GENERATORS = bin/classic_aes bin/erasures bin/alternative_aes bin/noise
BINS = $(CORRECTORS) $(GENERATORS)

all: $(BINS)

correct: $(CORRECTORS)

gen: $(GENERATORS)


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

bin/%: src/generators/%.c $(OBJ)
	mkdir -p bin
	gcc -g -o $@ $^ $(FLAGS)
