#include <iostream>
#include <fstream>
#include <string>

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

    std::ofstream outputFile("messages.txt", std::ios::app);

    if (!outputFile) {
        std::cerr << "Error: Could not open storage file." << std::endl;
        return 1;
    }

    outputFile << message << std::endl;
    outputFile.close();

    std::cout << "Message saved successfully." << std::endl;

    return 0;
}
