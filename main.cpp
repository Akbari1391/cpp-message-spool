#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

std::queue<std::string> messageQueue;

std::mutex queueMutex;
std::mutex fileMutex;
std::mutex outputMutex;

std::condition_variable queueCondition;

bool finished = false;

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

        {
            std::lock_guard<std::mutex> lock(fileMutex);

            std::ofstream outputFile("messages.txt", std::ios::app);

            if (!outputFile) {
                std::lock_guard<std::mutex> outputLock(outputMutex);
                std::cerr << "Error: Could not open storage file."
                          << std::endl;
                continue;
            }

            outputFile << message << std::endl;
        }

        {
            std::lock_guard<std::mutex> lock(outputMutex);

            std::cout << "Worker " << workerId
                      << " processed: " << message
                      << std::endl;
        }
    }
}

int main() {
    std::string message;

    std::cout << "Previously saved messages:" << std::endl;

    std::ifstream inputFile("messages.txt");

    if (inputFile) {
        while (std::getline(inputFile, message)) {
            std::cout << "- " << message << std::endl;
        }
    }

    constexpr int workerCount = 3;
    std::vector<std::thread> workers;

    for (int i = 1; i <= workerCount; ++i) {
        workers.emplace_back(processMessages, i);
    }

    std::cout << "\nEnter messages (type 'done' to finish):"
              << std::endl;

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
