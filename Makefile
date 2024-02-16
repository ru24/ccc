CFLAGS=-std=c11 -g -static
SRCS=main.c parse.c codegen.c tokenize.c
OBJS=$(SRCS:.c=.o)

ccc: $(OBJS)
	$(CC) -o ccc $(OBJS) $(LDFLAGS)

$(OBJS): ccc.h

test: ccc
	./test.sh

clean:
	rm -f ccc *.o *~ tmp*

.PHONY: test clean