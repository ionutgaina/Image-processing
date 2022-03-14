build:
	gcc bmp.c -o bmp -lm -g

run:
	./bmp

clean:
	rm -f bmp