#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <vector>

std::queue<std::string> messageQueue;
std::mutex queueMutex;
std::mutex fileMutex;

void processMessages(int workerId) {
    while (true) {
        std::string message;

        {
            std::lock_guard<std::mutex> lock(queueMutex);

            if (messageQueue.empty()) {
                return;
            }

            message = messageQueue.front();
            messageQueue.pop();
        }

        {
            std::lock_guard<std::mutex> lock(fileMutex);

            std::ofstream outputFile("messages.txt", std::ios::app);

            if (!outputFile) {
                std::cerr << "Error: Could not open storage file."
                          << std::endl;
                return;
            }

            outputFile << message << std::endl;
        }

        std::cout << "Worker " << workerId
                  << " processed: " << message << std::endl;
    }
}

int main() {
    std::string message;

    std::cout << "Previously saved messages:" << std::endl;

    std::ifstream inputFile("messages.txt");
    while (std::getline(inputFile, message)) {
        std::cout << "- " << message << std::endl;
    }

    std::cout << "\nEnter messages (type 'done' to finish):"
              << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, message);

        if (message == "done") {
            break;
        }

        std::lock_guard<std::mutex> lock(queueMutex);
        messageQueue.push(message);
    }

    std::vector<std::thread> workers;

    for (int i = 1; i <= 3; ++i) {
        workers.emplace_back(processMessages, i);
    }

    for (auto& worker : workers) {
        worker.join();
    }

    std::cout << "All messages processed." << std::endl;

    return 0;
}
