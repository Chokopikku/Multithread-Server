CC=gcc
CFLAGS=-g -pthread
BINS=server
all: $(BINS)

%: %.C
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf *.dSYM $(BINS)
