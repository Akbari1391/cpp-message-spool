#include "message_spool.h"

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

std::queue<std::string> messageQueue;

std::mutex queueMutex;
std::mutex storageMutex;
std::mutex outputMutex;

std::condition_variable queueCondition;

bool finished = false;

MessageSpool spool("messages.txt");

void processMessages(int workerId) {
    while (true) {
        std::string message;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            queueCondition.wait(lock, [] {
                return !messageQueue.empty() || finished;
            });

            if (messageQueue.empty() && finished) {
                return;
            }

            message = messageQueue.front();
            messageQueue.pop();
        }

        bool saved = false;

        {
            std::lock_guard<std::mutex> lock(storageMutex);
            saved = spool.saveMessage(message);
        }

        {
            std::lock_guard<std::mutex> lock(outputMutex);

            if (saved) {
                std::cout << "Worker " << workerId
                          << " processed: " << message
                          << std::endl;
            } else {
                std::cerr << "Error: Could not save message."
                          << std::endl;
            }
        }
    }
}

int main() {
    std::cout << "Previously saved messages:" << std::endl;

    for (const auto& message : spool.loadMessages()) {
        std::cout << "- " << message << std::endl;
    }

    constexpr int workerCount = 3;
    std::vector<std::thread> workers;

    for (int i = 1; i <= workerCount; ++i) {
        workers.emplace_back(processMessages, i);
    }

    std::cout << "\nEnter messages (type 'done' to finish):"
              << std::endl;

    std::string message;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, message);

        if (message == "done") {
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                finished = true;
            }

            queueCondition.notify_all();
            break;
        }

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            messageQueue.push(message);
        }

        queueCondition.notify_one();
    }

    for (auto& worker : workers) {
        worker.join();
    }

    std::cout << "All messages processed." << std::endl;

    return 0;
}