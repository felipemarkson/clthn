CFLAGS = -Wall -Wextra -Werror

.PHONY: debug release build clean main controller index api httpserver

debug: CFLAGS += -O0 -ggdb -DDEBUG
debug: build

release: CFLAGS += -O3
release: build
	strip main

build: main controller httpserver
	cc *.o -o main -levent

clean:
	rm -f *.o
	rm -f main

# Modules
main: main.o

controller: index api controller.o utils.o 

index: index.o utils.o

api: api.o utils.o

httpserver: httpserver.o


# Translation units
main.o:
	cc $(CFLAGS) -c src/main.c

controller.o:
	cc $(CFLAGS) -c src/controller/controller.c

httpserver.o:
	cc $(CFLAGS) -c src/httpserver/httpserver.c

api.o:
	cc $(CFLAGS) -c src/controller/api/api.c

index.o:
	cc $(CFLAGS) -c src/controller/index/index.c

utils.o:
	cc $(CFLAGS) -c src/controller/utils.c