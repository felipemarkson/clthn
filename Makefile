CFLAGS = -Wall -Wextra -Werror

debug: CFLAGS += -O0 -ggdb
debug: build

release: CFLAGS += -O3
release: build
	strip main

build: main.o controller.o httpserver.o
	cc main.o controller.o httpserver.o -o main -levent

main.o:
	cc $(CFLAGS) -c src/main.c

controller.o:
	cc $(CFLAGS) -c src/controller/controller.c

httpserver.o:
	cc $(CFLAGS) -c src/httpserver/httpserver.c

clean:
	rm -f *.o
	rm -f main