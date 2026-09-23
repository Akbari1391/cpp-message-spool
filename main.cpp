#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::string message;

    std::cout << "Enter a message: ";
    std::getline(std::cin, message);

    std::ofstream file("messages.txt", std::ios::app);

    if (!file) {
        std::cerr << "Error: Could not open storage file." << std::endl;
        return 1;
    }

    file << message << std::endl;
    file.close();

    std::cout << "Message saved successfully." << std::endl;

    return 0;
}
