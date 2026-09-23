#include "message_spool.h"

#include <fstream>

MessageSpool::MessageSpool(const std::string& storageFile)
    : storageFile_(storageFile) {
}

bool MessageSpool::saveMessage(const std::string& message) {
    std::ofstream outputFile(storageFile_, std::ios::app);

    if (!outputFile) {
        return false;
    }

    outputFile << message << '\n';

    return static_cast<bool>(outputFile);
}

std::vector<std::string> MessageSpool::loadMessages() const {
    std::vector<std::string> messages;
    std::ifstream inputFile(storageFile_);
    std::string message;

    if (!inputFile) {
        return messages;
    }

    while (std::getline(inputFile, message)) {
        messages.push_back(message);
    }

    return messages;
}