all: main aes.o

%.o: %.c %.h
	gcc -c $<

%: %.c aes.o
	gcc -o $@ $^

clean:
	rm *.o main