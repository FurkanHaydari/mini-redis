# Mini Redis

## Overview

This project is a simple in-memory database similar to Redis, implemented in C. It is designed to help you enhance your skills in C programming, socket programming, and network communication. The project includes basic database operations such as CRUD (Create, Read, Update, Delete). It also features a simple Redis client written in Python to interact with the C-based Redis server.

## Features

- **In-Memory Database**: Implements a simple in-memory database with basic CRUD operations.
- **AVL Tree Structure**: Utilizes an AVL tree structure for data storage, ensuring balanced and fast data access.
- **Socket Programming**: Employs TCP/IP sockets for data exchange between the server and client.
- **Client Implementation**: A simple Redis client is implemented in Python, allowing for connection to the database and testing of data operations.
- **Logging**: Efficient logging and data tracking are achieved using syslog and a circular buffer for server log management.
- **Fault Tolerance Testing**: Tests for server stability and fault tolerance.
- **Performance Testing**: Measures performance and memory usage.

## Setup

1.  **Clone the Repository**:
    ```bash
    git clone https://github.com/furkanhaydari/mini-redis.git
    cd mini-redis
    ```
2.  **Compile the Server:** This command compiles the source files and links them to create the executable.

    ```bash
    make
    ```

3.  **Run the Server:** This command builds the project (if not already built) and then runs the executable.

        ```bash
        make run
        ```

    Or you can simply use:

        ```bash
        ./mini-redis
        ```

4.  **Clean Build Artifacts:** This command removes object files and the executable to clean the build environment.

    ```bash
    make clean
    ```

5.  **Run the Tests:** You can run various tests using the `tests/tesy.py` file. Works with Python 3:

    ```bash
    python3 tests/tesy.py
    ```

## Client Usage

The Python client interacts with the server using the following commands:

### Example Commands

- **SET key value**: Adds a key-value pair to the database.

  ```python
  send_command("SET key value")
  ```

- **GET key**: Retrieves the value of a specific key from the database.

  ```python
  send_command("GET key")
  ```

- **DEL key**: Deletes a specific key from the database.

  ```python
  send_command("DEL key")
  ```

### Syslog and Circular Buffer

The server logs all operations using syslog if the -s argument is provided. Otherwise, it logs to the console. Logging is managed with a circular buffer, which enables efficient cyclic management of log data and quick access to historical log information.

## Tests

The project includes various tests:

- **Basic Operations**: Tests the correct functionality of the SET, GET, and DEL commands.
- **Resize AVL Tree**: Tests the AVL tree resizing and ensures data integrity by adding and retrieving multiple keys.
- **Memory Leak**: Checks for memory leaks using `tracemalloc`.
- **Performance**: Measures the time required to perform a large number of SET operations.
- **Connection Handling**: Simulates multiple concurrent clients to test connection handling and server stability.
- **Fault Tolerance**: Tests server behavior under connection loss and network delay scenarios.

## Advanced Topics

- **SQL Parser**: Future enhancements include implementing an SQL parser and exploring Abstract Syntax Tree (AST) for query processing.
- **Synchronization Mechanisms**: Plans to explore synchronization mechanisms and PostgreSQL for advanced database functionalities.

## Contributions

Feel free to contribute by opening issues or submitting pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

Furkan Haydari - [LinkedIn](https://www.linkedin.com/in/furkan-haydari/) - [Email](mailto:furkanhydri@gmail.com)
