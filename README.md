# Mini Redis

## Overview

This project is a simple in-memory database similar to Redis, implemented in C. It is designed to help you enhance your skills in C programming, socket programming, and network communication. The project includes basic database operations such as CRUD (Create, Read, Update, Delete). It also features a simple Redis client written in Python to interact with the C-based Redis server.

## Features

- **In-Memory Database**: Implements a simple in-memory database with basic CRUD operations.
- **AVL Tree Structure**: Utilizes an AVL tree structure for data storage, ensuring balanced and fast data access.
- **Socket Programming**: Employs TCP/IP sockets for data exchange between the server and client.
- **Client Implementation**: A simple Redis client is implemented in Python for testing data operations.
- **Logging**: Efficient logging using syslog or console logging, with a circular buffer for server log management.
- **Fault Tolerance Testing**: Tests for server stability and fault tolerance.
- **Performance Testing**: Measures performance and memory usage.

## Setup and Usage

### Using Docker (Recommended)

1. **Build the Docker image:**

   ```bash
   make docker-build
   ```

2. **Run tests in Docker:**

   ```bash
   make docker-test
   ```

3. **Run the server in Docker:**
   ```bash
   docker run -p 45234:45234 mini-redis
   ```

### Manual Setup

1. **Compile the Server:**

   ```bash
   make
   ```

2. **Run the Server:**

   ```bash
   ./mini-redis [-p port] [-i] [-s]
   ```

   - `-p port`: Specify the port number (default is 45234)
   - `-i`: Set log level to INFO (default is ERROR)
   - `-s`: Use syslog for logging (default is console logging)

3. **Run Tests:**

   ```bash
   make test
   ```

4. **Clean Build Artifacts:**
   ```bash
   make clean
   ```

## API Usage

The server accepts JSON-formatted commands. Here are the basic operations:

### SET

Adds a key-value pair to the database.

```json
{"key": "mykey", "operation": "SET", "value": "myvalue"}
```

### GET

Retrieves the value of a specific key from the database.

```json
{"key": "mykey", "operation": "GET"}
```

### DEL

Deletes a specific key from the database.

```json
{"key": "mykey", "operation": "DEL"}
```

## Testing

The project includes various tests:

- Basic CRUD operations
- AVL tree resizing and data integrity
- Memory leak detection
- Performance measurement
- Fault tolerance
- Randomized operations

Tests are automatically run in a Docker container to ensure a consistent environment and prevent system interference. You can run the tests using:

```bash
make docker-test
```

## Advanced Topics

- **SQL Parser**: Future plans include implementing an SQL parser and exploring Abstract Syntax Tree (AST) for query processing.
- **Synchronization Mechanisms**: Plans to explore synchronization mechanisms and PostgreSQL for advanced database functionalities.

## Contributions

Contributions are welcome. Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

Furkan Haydari - [LinkedIn](https://www.linkedin.com/in/furkan-haydari/) - [Email](mailto:furkanhydri@gmail.com)
