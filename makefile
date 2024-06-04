FLAGS = -Iinclude -Wall -lm
BINS = bin/keygen bin/keymod bin/correct_bec bin/correct_z bin/new_schedule bin/correct_alt
all: $(BINS)

obj/%.o: src/%.c include/%.h
	mkdir -p obj
	gcc -g -c $< -o $@ $(FLAGS)

bin/%: src/%.c obj/aes.o obj/util.o obj/list.o
	mkdir -p bin
	gcc -g -o $@ $^ $(FLAGS)

clean:
	rm -f obj/* bin/*