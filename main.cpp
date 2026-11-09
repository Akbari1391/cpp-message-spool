#include "message_spool.h"

#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

struct QueuedMessage {
    std::uint64_t id;
    std::string content;
};

std::queue<QueuedMessage> messageQueue;
std::mutex queueMutex;
std::mutex outputMutex;
std::condition_variable queueCondition;

bool finished = false;
bool processingFailed = false;

MessageSpool spool("messages.txt");

void processMessages(int workerId) {
    while (true) {
        QueuedMessage message;

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

        const bool saved = spool.saveMessage(message.content);
        bool acknowledged = false;

        if (saved) {
            acknowledged = spool.acknowledgeMessage(message.id);
        }

        {
            std::lock_guard<std::mutex> lock(outputMutex);

            if (saved && acknowledged) {
                std::cout << "Worker " << workerId
                          << " processed message " << message.id
                          << ": " << message.content << '\n';
            } else {
                std::cerr << "Worker " << workerId
                          << " failed to complete message "
                          << message.id << '\n';

                processingFailed = true;
            }
        }
    }
}

int main() {
    std::cout << "Previously saved messages:\n";

    for (const auto& message : spool.loadMessages()) {
        std::cout << "- " << message << '\n';
    }

    std::vector<MessageRecord> recovered;

    if (!spool.loadPendingMessages(recovered)) {
        std::cerr
            << "Error: Cannot read message journal.\n"
            << "The journal may be missing, unreadable, or corrupted.\n"
            << "Processing stopped to prevent data loss.\n";

        return 1;
    }

    std::cout << "Recovered " << recovered.size()
              << " pending messages.\n";

    {
        std::lock_guard<std::mutex> lock(queueMutex);

        for (const auto& message : recovered) {
            messageQueue.push({message.id, message.content});
        }
    }

    constexpr int workerCount = 3;
    std::vector<std::thread> workers;

    for (int i = 1; i <= workerCount; ++i) {
        workers.emplace_back(processMessages, i);
    }

    std::cout << "\nEnter messages (type 'done' to finish):\n";

    std::string message;

    while (true) {
        std::cout << "> ";

        if (!std::getline(std::cin, message) || message == "done") {
            break;
        }

        if (message.empty()) {
            continue;
        }

        std::uint64_t messageId = 0;

        if (!spool.enqueueMessage(message, messageId)) {
            std::cerr
                << "Error: Could not enqueue message.\n"
                << "Check journal integrity and file permissions.\n";

            processingFailed = true;
            break;
        }

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            messageQueue.push({messageId, message});
        }

        queueCondition.notify_one();
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        finished = true;
    }

    queueCondition.notify_all();

    for (auto& worker : workers) {
        worker.join();
    }

    std::size_t remaining = 0;

    if (!spool.pendingCount(remaining)) {
        std::cerr << "Error: Cannot verify pending messages.\n";
        return 1;
    }

    if (processingFailed || remaining != 0) {
        std::cerr << "Warning: " << remaining
                  << " pending messages remain.\n";

        return 1;
    }

    std::cout << "All messages processed successfully.\n";

    return 0;
}
