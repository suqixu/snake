all:
	gcc -Wall -O2 snake.c -lSDL -lSDLmain -o snake

clean:
	rm snake tags

tags:
	ctags R snake.c

