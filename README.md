# C++ Message Spool

A small C++17 project for practicing message persistence, multithreading, and thread-safe queue processing on Linux.

## Features

- Stores messages persistently in a text file
- Loads previously saved messages on startup
- Uses a FIFO message queue
- Processes messages using multiple worker threads
- Uses mutexes for thread-safe queue and file access
- Built and tested on Linux

## Technologies

- C++17
- Linux
- STL
- `std::queue`
- `std::thread`
- `std::mutex`
- File I/O

## Build

Compile the program with:

```bash
g++ -std=c++17 -pthread main.cpp -o message-spool
```

## Run

Run the program with:

```bash
./message-spool
```

Enter messages one at a time. Type `done` when you are finished entering messages.

Example:

```text
Previously saved messages:
- Hello from my first C++ message spool
- Second message test

Enter messages (type 'done' to finish):
> Message one
> Message two
> Message three
> done

Worker 1 processed: Message one
Worker 2 processed: Message two
Worker 2 processed: Message three
All messages processed.
```

Messages are stored in `messages.txt` and loaded again the next time the program starts.

## How It Works

1. Previously stored messages are loaded from `messages.txt`.
2. New messages are added to a FIFO queue.
3. Multiple worker threads process messages from the queue.
4. Mutexes protect shared queue and file operations.
5. Processed messages are appended to persistent storage.

## Purpose

This is a learning project focused on strengthening practical C++ and Linux skills while exploring basic concepts used in reliable message-processing systems.
