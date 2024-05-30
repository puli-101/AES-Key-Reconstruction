FLAGS = -Iinclude
BINS = keygen keymod correct_bec
all: $(BINS)

obj/%.o: src/%.c include/%.h
	mkdir -p obj
	gcc -g -c $< -o $@ $(FLAGS)

%: src/%.c obj/aes.o obj/util.o
	mkdir -p bin
	gcc -g -o bin/$@ $^ $(FLAGS)

clean:
	rm -f obj/* bin/*