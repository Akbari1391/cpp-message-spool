# C++ Message Spool

A C++17 message-processing project built to practice Linux development, message persistence, multithreading, synchronization, automated testing, and CI.

The project implements a producer-consumer model in which multiple worker threads wait for incoming messages, process them concurrently, and persist them to disk.

## Features

- Producer-consumer message processing
- Multiple concurrent worker threads
- Thread-safe FIFO message queue
- `std::condition_variable` for worker notification
- `std::mutex` for synchronized queue and file access
- Persistent message storage using file I/O
- Loads previously stored messages on startup
- Graceful worker shutdown
- Automated FIFO queue testing
- Automated persistence testing
- CMake build configuration
- GitHub Actions continuous integration
- Built and tested on Linux

## Technologies

- C++17
- Linux
- CMake
- STL
- `std::queue`
- `std::thread`
- `std::mutex`
- `std::condition_variable`
- File I/O
- CTest
- Git
- GitHub Actions

## Architecture

The application follows a basic producer-consumer design.

The main thread acts as the producer and accepts messages from the user. Each message is added to a shared FIFO queue.

Multiple worker threads act as consumers. Workers wait on a condition variable instead of continuously checking the queue.

When a new message is added:

1. The producer locks the queue.
2. The message is pushed into the FIFO queue.
3. A waiting worker is notified.
4. The worker safely removes the message from the queue.
5. The message is written to persistent storage.
6. The worker waits for another message.

Mutexes protect shared resources and prevent concurrent access to the queue and storage file.

## Build

Configure the project:

```bash
cmake -S . -B build
```

Build the project:

```bash
cmake --build build
```

## Run

Run the application:

```bash
./build/message-spool
```

Enter messages one at a time.

Example:

```text
Enter messages (type 'done' to finish):
> Message one
Worker 1 processed: Message one
> Message two
Worker 2 processed: Message two
> Message three
Worker 3 processed: Message three
> done
All messages processed.
```

Messages are stored in `messages.txt` and loaded again when the application is restarted.

## Automated Tests

The project currently includes two automated tests:

- `QueueFIFOTest` verifies FIFO queue behavior.
- `PersistenceTest` verifies that a message can be written to storage and read back correctly.

Run all tests with:

```bash
ctest --test-dir build --output-on-failure
```

Example result:

```text
100% tests passed, 0 tests failed out of 2
```

## Continuous Integration

GitHub Actions automatically builds and tests the project on Ubuntu for pushes and pull requests to the `main` branch.

The CI workflow performs:

1. Repository checkout
2. CMake configuration
3. Project build
4. Automated tests with CTest

## Project Structure

```text
cpp-message-spool/
├── .github/
│   └── workflows/
│       └── ci.yml
├── CMakeLists.txt
├── main.cpp
├── test.cpp
├── persistence_test.cpp
├── .gitignore
├── LICENSE
└── README.md
```

Runtime files such as `messages.txt` and build artifacts are excluded from version control.

## Purpose

This is a learning project focused on strengthening practical C++ and Linux development skills while exploring concepts used in reliable message-processing systems, including concurrency, synchronization, persistence, automated testing, and continuous integration.