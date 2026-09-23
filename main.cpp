#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>

std::queue<std::string> messageQueue;
std::mutex queueMutex;

void processMessage() {
    std::lock_guard<std::mutex> lock(queueMutex);

    if (!messageQueue.empty()) {
        std::ofstream outputFile("messages.txt", std::ios::app);

        if (!outputFile) {
            std::cerr << "Error: Could not open storage file." << std::endl;
            return;
        }

        outputFile << messageQueue.front() << std::endl;

        std::cout << "Message processed by worker thread: "
                  << messageQueue.front() << std::endl;

        messageQueue.pop();
    }
}

int main() {
    std::string message;

    std::cout << "Previously saved messages:" << std::endl;

    std::ifstream inputFile("messages.txt");
    while (std::getline(inputFile, message)) {
        std::cout << "- " << message << std::endl;
    }
    inputFile.close();

    std::cout << "\nEnter a new message: ";
    std::getline(std::cin, message);

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        messageQueue.push(message);
    }

    std::thread worker(processMessage);
    worker.join();

    return 0;
}
