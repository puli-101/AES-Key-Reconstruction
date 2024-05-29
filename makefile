FLAGS = -Iinclude
BINS = keygen corruption correct_bec
all: $(BINS)

obj/%.o: src/%.c include/%.h
	mkdir -p obj
	gcc -c $< -o $@ $(FLAGS)

%: src/%.c obj/aes.o obj/util.o
	mkdir -p bin
	gcc -o bin/$@ $^ $(FLAGS)

clean:
	rm obj/* bin/*