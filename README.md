# NetGrep

English | [Russian](./README_ru.md)

A high-performance client-server service for concurrent text signature matching in large files. Written in C++20 using POSIX sockets, multi-process architecture (`fork`), and lock-free statistics via Shared Memory.

## 🚀 Architectural Decisions

The project consists of three independent components: Client, Server, and Statistics Utility.

- **Memory and Network:** To simplify the virus search algorithm (to avoid handling signature "breaks" at the boundaries of network chunks), the file is read entirely into RAM (into `std::string`). However, network transmission is strictly chunked (in 10 MB pieces). This solves the macOS/Linux kernel internal limits on the maximum buffer size in the `send()` system call (`EINVAL` error on gigabyte files). The server receives data using a 64 KB buffer to minimize context switches.

- **Multi-processing:** The server uses the classic `fork()` model for each new incoming connection. Child processes scan files concurrently and independently of each other. Safe descriptor closure is implemented through a custom RAII wrapper, `SocketFD`.

- **Lock-free Statistics:** Workers write scanning results to POSIX Shared Memory (`shm_open` + `mmap`) using `std::atomic`. The `statistics` utility can connect to this named memory segment at any time and read the current data in real-time, completely without blocking the main server's operation.

- **Testing (GTest):** End-to-End integration tests have been written. The test server is spun up in a separate thread and binds to a dynamically allocated free port by the OS (port `0`), which prevents conflicts (Address already in use) during parallel pipeline execution in CI/CD (GitHub Actions).

## 🛠 Build Instructions

The project uses CMake. Building on Linux and macOS is supported.

```bash
# Clone and navigate to the directory
git clone git@github.com:Des10Tiny/NetGrep.git
cd NetGrep

# Generate and build (Release with max optimization)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## 💻 Usage

**Start the server**
The server takes a port and a path to the configuration file with virus signatures (each signature on a new line).

```bash
./build/src/server 8080 ./tests/test_server/data/config.txt
```

**Send a file (Client)**
Send a file for scanning. The client will wait for the response and output the verdict (Clean / Infected).

```bash
./build/src/client ./tests/test_client/data/infected.txt 8080
```

**View statistics**
Outputs aggregated information on all scanned files without interrupting the server.

```bash
./build/src/statistics ./tests/test_server/data/config.txt
```

## 🧪 Running Tests

Tests are written using Google Test.

```bash
cd build
ctest --output-on-failure
```

---

🔗 **GitHub Repository:** [Des10Tiny](https://github.com/Des10Tiny)
