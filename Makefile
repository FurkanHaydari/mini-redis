CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -I/usr/include/json-c
LDFLAGS = -ljson-c
SRC = src/main.c src/server.c src/database.c src/log.c src/log_syslog.c src/circular_buffer.c
OBJ = $(SRC:.c=.o)
EXEC = mini-redis
TEST_EXEC = pytest ./tests/test.py -v

# Default target to build the project
all: $(EXEC)

# Target to build the executable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

# Rule to build object files from source files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Target to clean build artifacts
clean:
	rm -f $(OBJ) $(EXEC)

# Target to run the executable
run: $(EXEC)
	./$(EXEC)

# Target to run tests
test: $(EXEC)
	@echo "Running tests..."
	./$(EXEC) -p 45234 &
	sleep 5
	@echo "Server PID: $$!"
	@netstat -tuln | grep 45234 || echo "Port 45234 is not in use"
	. /app/venv/bin/activate && pytest ./tests/test.py -v
	@echo "Tests completed."
	pkill mini-redis || true

# Target to build and run with Docker
docker-build:
	docker build -t mini-redis .

# Add a new target for building and running tests in Docker
docker-test:
	docker build --no-cache --target tester -t mini-redis-test .
	docker run --rm mini-redis-test make test || (echo "Tests failed" && exit 1)