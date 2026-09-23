#include <iostream>
#include <string>

int main() {
    std::string message;

    std::cout << "Enter a message: ";
    std::getline(std::cin, message);

    std::cout << "Message received: " << message << std::endl;

    return 0;
}
