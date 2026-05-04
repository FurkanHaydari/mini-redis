<div align="center">
  
# ⚡ Mini Redis
  
**A high-performance, in-memory database engine written entirely in C.**

[![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)](https://en.cppreference.com/w/c)
[![Python Client](https://img.shields.io/badge/Python_Client-3776AB?style=for-the-badge&logo=python&logoColor=white)](https://www.python.org/)
[![Docker](https://img.shields.io/badge/Docker_Ready-2496ED?style=for-the-badge&logo=docker&logoColor=white)](https://www.docker.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.style=for-the-badge)](https://opensource.org/licenses/MIT)

</div>

## 📖 Overview

**Mini Redis** is a custom implementation of an in-memory database, engineered from the ground up in C. Designed to demonstrate low-level systems programming, custom memory management, and TCP/IP network socket communication. 

This project explores the fundamental mechanics behind modern caching systems by implementing core database operations (CRUD), employing AVL trees for guaranteed *O(log n)* time complexities, and handling client-server network protocols autonomously without relying on high-level external frameworks.

---

## 🚀 Key Architectural Features

- **🧠 In-Memory Storage Engine**: Blazing-fast state retention utilizing heavily optimized manual memory allocations.
- **🌳 AVL Tree Indexing**: Ensures that all search, insert, and delete operations strictly adhere to *O(log n)* time complexities through automatic self-balancing tree rotations.
- **🌐 Custom Socket Networking**: Implements raw TCP/IP sockets for robust client-server architecture.
- **🐍 Lightweight Python Client**: Includes a custom protocol wrapper written in Python for effortless database integration and testing.
- **🛡️ Fault Tolerance & Memory Safety**: Architected to mitigate memory leaks with rigorous Valgrind-tested allocations and graceful crash-handling policies.
- **📝 High-Performance Logging**: Employs an intelligent circular-buffer logging strategy (syslog and console modes) for bottleneck-free analytics.

---

## 🛠️ Setup and Usage

### 🐳 The Docker Way (Recommended)

1. **Build the container:**
   ```bash
   make docker-build
   ```
2. **Run integration tests inside isolated container:**
   ```bash
   make docker-test
   ```
3. **Spin up the server:**
   ```bash
   docker run -p 45234:45234 mini-redis
   ```

### 💻 The Manual Way (Bare-metal)

1. **Compile via Make:**
   ```bash
   make
   ```
2. **Run the Server:**
   ```bash
   ./mini-redis [-p port] [-i] [-s]
   ```
   * `-p port`: Specify network port (default: `45234`)
   * `-i`: Enable `INFO` logging verbosity (default: `ERROR`)
   * `-s`: Bind to `syslog` instead of standard console output.

3. **Run Unit Tests & Cleanup:**
   ```bash
   make test
   make clean
   ```

---

## 🔌 API & Protocol

The server communicates via standard JSON payloads over TCP.

| Operation | Command Example | Description |
| :--- | :--- | :--- |
| **SET** | `{"operation": "SET", "key": "mykey", "value": "myvalue"}` | Inserts or updates a key-value pair. |
| **GET** | `{"operation": "GET", "key": "mykey"}` | Retrieves the value bound to the key. |
| **DEL** | `{"operation": "DEL", "key": "mykey"}` | Removes the key and frees memory. |

---

## 🧪 Testing & Validation

To guarantee memory safety and architectural integrity, the project is equipped with a rigorous test suite:
- **Tree Integrity:** Validates strict AVL height rules and rebalancing (Left-Left, Right-Right, Left-Right, Right-Left).
- **Leak Detection:** Monitors dangling pointers during rapid CRUD cycles.
- **Fault Tolerance:** Randomizes socket closures and invalid payloads to ensure the main thread never crashes.

Run the test suite safely via Docker:
```bash
make docker-test
```

---

## 🔮 Roadmap & Advanced Topics

- **[ ] SQL Parser:** Implementation of Abstract Syntax Trees (AST) for relational-style complex queries.
- **[ ] Concurrency:** Migrating the event loop to `epoll` or `kqueue` for high-concurrency non-blocking I/O.
- **[ ] Disk Persistence:** AOF (Append Only File) logging for crash recovery.

---

## 🤝 Contributions
Pull requests are welcome! If you want to optimize the AVL rotations or add multi-threading, feel free to submit a PR.

<div align="center">
  <i>Developed by <b>Furkan Haydari</b></i> <br>
  <a href="https://www.linkedin.com/in/furkan-haydari/">LinkedIn</a> • <a href="mailto:furkanhydri@gmail.com">Email</a> • <a href="https://github.com/FurkanHaydari">GitHub</a>
</div>
