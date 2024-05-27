all: main

%.o: %.c %.h
	gcc -c $<

%: %.c aes.o
	gcc -o $@ $< 