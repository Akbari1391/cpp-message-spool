# C++ Message Spool

A C++17 concurrent message-processing application featuring persistent message storage, an append-only message journal, restart recovery, automated testing, and continuous integration.

The project demonstrates practical systems programming concepts, including multithreading, synchronization, file-based persistence, message lifecycle management, and error detection.

## Key Features

- Multithreaded producer-consumer architecture
- Three concurrent message-processing workers
- Thread-safe FIFO work queue
- Synchronization using mutexes and condition variables
- Append-only message journal
- Unique message identifiers
- Pending-message recovery after application restart
- Message acknowledgement tracking
- Journal corruption detection
- Graceful worker shutdown
- CMake and CTest integration
- Automated Linux builds and tests with GitHub Actions

## Technology Stack

- C++17
- Linux
- CMake
- CTest
- C++ Standard Library
- Git and GitHub Actions

## System Architecture

The application uses a producer-consumer architecture.

The main thread accepts messages and records them in a persistent journal before placing them into a shared work queue.

Three worker threads retrieve messages from the queue and process them concurrently.

The processing workflow is:

1. Accept a message from the user.
2. Assign a unique message identifier.
3. Append an enqueue record to the journal.
4. Place the message into the thread-safe work queue.
5. Retrieve the message using an available worker.
6. Save the processed message to persistent storage.
7. Append an acknowledgement record to the journal.

Mutexes protect shared resources, while condition variables coordinate worker activity.

## Persistent Message Journal

The `MessageSpool` component manages persistent message storage and journal operations.

The application uses two runtime files:

- `messages.txt` — Stores processed message content.
- `messages.txt.journal` — Tracks message enqueue and acknowledgement operations.

Journal records use message identifiers to track processing state.

Pending messages can be reconstructed by replaying valid journal records.

### Restart Recovery

When the application starts, it reads the journal and identifies messages that have not been acknowledged.

These messages are restored to the processing queue.

If the journal contains an invalid record, the application reports an error rather than silently treating the journal as empty.

A missing journal is treated as a fresh application initialization.

### Reliability Limitations

The current implementation provides restart recovery for valid journal files but does not guarantee exactly-once processing.

A message may be written to the output file more than once if the application stops after saving it but before recording its acknowledgement.

File-stream flushing is used, but power-loss durability is not guaranteed through operating-system-level synchronization.

These limitations are important considerations for future development.

## Build Instructions

Requirements:

- C++17-compatible compiler
- CMake 3.16 or newer
- Linux development environment

Configure:

```bash
cmake -S . -B build
```

Build:

```bash
cmake --build build
```

## Running the Application

Start the program:

```bash
./build/message-spool
```

Enter messages interactively.

Example:

```text
Enter messages (type 'done' to finish):
> Message one
> Message two
> Message three
> done
All messages processed successfully.
```

Worker processing messages may also appear during interactive input.

Type `done` to stop accepting messages and allow queued work to finish.

## Automated Testing

The project includes two CTest targets.

### QueueFIFOTest

Verifies FIFO behavior using the C++ standard queue.

### PersistenceTest

Exercises the production `MessageSpool` component, including:

- Initial startup without an existing journal
- Persistent message storage and retrieval
- Message enqueue operations
- Unique message identifier generation
- Message acknowledgements
- Pending-message recovery after object recreation
- Identifier continuity after acknowledgements
- Detection of invalid journal records

Run the tests:

```bash
ctest --test-dir build --output-on-failure
```

Latest verified CI result:

```text
100% tests passed, 0 tests failed out of 2
```

## Continuous Integration

GitHub Actions automatically builds and tests the application.

The CI workflow includes:

1. Repository checkout
2. CMake configuration
3. C++ compilation
4. Automated test execution with CTest

The project has been successfully built and tested using GitHub-hosted Linux runners.

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
├── persistence_test.cpp
├── test.cpp
├── .gitignore
├── LICENSE
└── README.md
```

## Engineering Concepts

This project demonstrates:

- Concurrent application development
- Producer-consumer design
- Thread synchronization
- Persistent data management
- Append-only journaling
- Message state tracking
- Restart recovery
- Error handling
- Automated regression testing
- Continuous integration

## Future Improvements

Potential enhancements include:

- Crash-consistent message completion
- Idempotent processing
- Journal checksums and recovery from incomplete records
- Operating-system-level durability guarantees
- Configurable worker pools
- Retry policies and dead-letter queues
- Structured logging and performance metrics
- Additional concurrency and failure-injection tests

## License

MIT License.
