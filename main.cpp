#include <iostream>
#include <fstream>
#include <string>
#include <queue>

int main() {
    std::queue<std::string> messageQueue;
    std::string message;

    std::cout << "Previously saved messages:" << std::endl;

    std::ifstream inputFile("messages.txt");
    while (std::getline(inputFile, message)) {
        std::cout << "- " << message << std::endl;
    }
    inputFile.close();

    std::cout << "\nEnter a new message: ";
    std::getline(std::cin, message);

    messageQueue.push(message);

    std::ofstream outputFile("messages.txt", std::ios::app);

    if (!outputFile) {
        std::cerr << "Error: Could not open storage file." << std::endl;
        return 1;
    }

    while (!messageQueue.empty()) {
        outputFile << messageQueue.front() << std::endl;
        std::cout << "Message processed: "
                  << messageQueue.front() << std::endl;

        messageQueue.pop();
    }

    outputFile.close();

    return 0;
}
