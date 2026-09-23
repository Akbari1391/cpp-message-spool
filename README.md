# C++ Message Spool

A C++17 message-processing project for practicing Linux development, concurrent message processing, persistence, automated testing, and continuous integration.

The application uses a producer-consumer design with multiple worker threads. Messages are placed into a thread-safe FIFO queue, processed by available workers, and persisted to disk.

## Features

- Producer-consumer message processing
- Multiple concurrent worker threads
- Thread-safe FIFO message queue
- Worker synchronization with `std::condition_variable`
- Shared-resource protection with `std::mutex`
- Persistent message storage
- Previously stored messages loaded on startup
- Graceful worker shutdown
- Reusable `MessageSpool` persistence component
- Automated FIFO queue testing
- Automated persistence testing against the production persistence component
- CMake build configuration
- CTest integration
- GitHub Actions CI
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

The application follows a producer-consumer model.

The main thread acts as the producer and accepts messages from the user. Each message is added to a shared FIFO queue.

Multiple worker threads act as consumers. Workers wait on a condition variable until a new message becomes available or the application begins shutting down.

When a message is submitted:

1. The producer locks the shared queue.
2. The message is added to the FIFO queue.
3. One waiting worker is notified.
4. The worker safely removes the next message from the queue.
5. The message is passed to the `MessageSpool` persistence component.
6. The message is written to disk.
7. The worker waits for the next available message.

Separate mutexes protect queue access, persistent storage operations, and console output.

When the user enters `done`, waiting workers are notified and the application performs a graceful shutdown after queued work has been processed.

## Persistence

Message persistence is separated from the application logic through the `MessageSpool` class.

`MessageSpool` provides operations to:

- Append messages to persistent storage
- Load previously stored messages
- Report whether a message was successfully saved

The application uses `messages.txt` as its runtime storage file.

Separating persistence from `main.cpp` also allows the same production component to be tested directly.

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

Because workers execute concurrently, the worker ID assigned to each message may vary between runs.

## Automated Tests

The project currently includes two automated tests:

- `QueueFIFOTest` verifies FIFO queue behavior.
- `PersistenceTest` exercises the production `MessageSpool` component by saving multiple messages and loading them back from temporary storage.

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
4. Automated testing with CTest

This helps verify that new changes continue to compile and pass the automated tests in a clean Linux environment.

## Project Structure

```text
cpp-message-spool/
├── .github/
│   └── workflows/
│       └── ci.yml
├── CMakeLists.txt
├── main.cpp
├── message_spool.h
├── message_spool.cpp
├── test.cpp
├── persistence_test.cpp
├── .gitignore
├── LICENSE
└── README.md
```

Runtime files such as `messages.txt`, compiled executables, and build artifacts are excluded from version control.

## Purpose

This is a learning project focused on strengthening practical C++ and Linux development skills while exploring concepts used in reliable message-processing systems, including concurrency, synchronization, persistence, modular design, automated testing, and continuous integration.