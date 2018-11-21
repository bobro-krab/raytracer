all:
	g++ -g -o bin/prog src/main.cpp -Wno-write-strings

run: all
	./bin/prog
	eog test.bmp

.PHONY: all
