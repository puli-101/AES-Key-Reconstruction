FLAGS = -Iinclude
all: keygen obj/aes.o corruption

obj/%.o: src/%.c include/%.h
	mkdir -p obj
	gcc -c $< -o $@ $(FLAGS)

%: src/%.c obj/aes.o
	mkdir -p bin
	gcc -o bin/$@ $^ $(FLAGS)

clean:
	rm obj/* bin/*