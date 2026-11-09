#include "message_spool.h"

#include <fstream>
#include <cstdio>

MessageSpool::MessageSpool(const std::string& storageFile)
    : storageFile_(storageFile),
      pendingFile_(storageFile + ".pending") {
}

bool MessageSpool::saveMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(fileMutex_);

    std::ofstream outputFile(storageFile_, std::ios::app);

    if (!outputFile) {
        return false;
    }

    outputFile << message << '\n';
    outputFile.flush();

    return static_cast<bool>(outputFile);
}

std::vector<std::string> MessageSpool::loadMessages() const {
    std::lock_guard<std::mutex> lock(fileMutex_);

    std::vector<std::string> messages;
    std::ifstream inputFile(storageFile_);
    std::string message;

    while (std::getline(inputFile, message)) {
        messages.push_back(message);
    }

    return messages;
}

bool MessageSpool::savePendingMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(fileMutex_);

    std::ofstream outputFile(pendingFile_, std::ios::app);

    if (!outputFile) {
        return false;
    }

    outputFile << message << '\n';
    outputFile.flush();

    return static_cast<bool>(outputFile);
}

std::vector<std::string> MessageSpool::loadPendingMessages() const {
    std::lock_guard<std::mutex> lock(fileMutex_);

    std::vector<std::string> messages;
    std::ifstream inputFile(pendingFile_);
    std::string message;

    while (std::getline(inputFile, message)) {
        messages.push_back(message);
    }

    return messages;
}

bool MessageSpool::clearPendingMessages() {
    std::lock_guard<std::mutex> lock(fileMutex_);

    return std::remove(pendingFile_.c_str()) == 0;
}
