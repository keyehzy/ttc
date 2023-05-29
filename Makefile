CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99 -g

ttc: ttc.c

clean:
	rm -f ttc
