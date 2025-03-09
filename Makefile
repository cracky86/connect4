all: connect4.c
	@gcc connect4.c -o connect4 -Ofast -lpthread
