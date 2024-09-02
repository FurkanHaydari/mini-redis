CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -I/usr/include/json-c  # Directory for JSON-C header files
LDFLAGS = -ljson-c  # Linker flag for JSON-C library
SRC = src/main.c src/server.c src/database.c src/log.c src/log_syslog.c src/circular_buffer.c
OBJ = $(SRC:.c=.o)
EXEC = mini-redis

# Default target to build the project
all: $(EXEC)

# Target to build the executable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)  # Add LDFLAGS for linking

# Rule to build object files from source files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Target to clean build artifacts
clean:
	rm -f $(OBJ) $(EXEC)

# Target to run the executable
run: $(EXEC)
	./$(EXEC)