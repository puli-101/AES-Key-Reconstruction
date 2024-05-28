FLAGS = -Iinclude
all: main obj/aes.o

obj/%.o: src/%.c include/%.h
	mkdir -p obj
	gcc -c $< -o $@ $(FLAGS)

%: src/%.c obj/aes.o
	mkdir -p bin
	gcc -o bin/$@ $^ $(FLAGS)

clean:
	rm obj/* bin/*