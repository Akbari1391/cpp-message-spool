#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

int main() {
    const std::string filename = "test_messages.txt";
    const std::string testMessage = "Persistent test message";

    // Write a message to the test file.
    {
        std::ofstream outputFile(filename, std::ios::app);

        assert(outputFile.is_open());

        outputFile << testMessage << std::endl;
    }

    // Read the message back from the test file.
    {
        std::ifstream inputFile(filename);

        assert(inputFile.is_open());

        std::string loadedMessage;
        std::getline(inputFile, loadedMessage);

        assert(loadedMessage == testMessage);
    }

    // Remove the temporary test file.
    std::remove(filename.c_str());

    std::cout << "Persistence test passed." << std::endl;

    return 0;
}