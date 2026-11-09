#include "message_spool.h"

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

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

    std::ifstream inputFile(pendingFile_);

    if (!inputFile) {
        return true;
    }

    inputFile.close();

    return std::remove(pendingFile_.c_str()) == 0;
}

bool MessageSpool::acknowledgePendingMessage(
    const std::string& message) {

    std::lock_guard<std::mutex> lock(fileMutex_);

    std::ifstream inputFile(pendingFile_);

    if (!inputFile) {
        return false;
    }

    std::vector<std::string> remaining;
    std::string current;
    bool removed = false;

    while (std::getline(inputFile, current)) {
        if (!removed && current == message) {
            removed = true;
        } else {
            remaining.push_back(current);
        }
    }

    if (inputFile.bad()) {
        return false;
    }

    inputFile.close();

    if (!removed) {
        return false;
    }

    const std::string tempFile = pendingFile_ + ".tmp";

    {
        std::ofstream outputFile(tempFile, std::ios::trunc);

        if (!outputFile) {
            return false;
        }

        for (const auto& item : remaining) {
            outputFile << item << '\n';
        }

        outputFile.flush();

        if (!outputFile) {
            return false;
        }
    }

    if (std::rename(tempFile.c_str(), pendingFile_.c_str()) != 0) {
        std::remove(tempFile.c_str());
        return false;
    }

    return true;
}

bool MessageSpool::recoverPendingMessages() {
    std::lock_guard<std::mutex> lock(fileMutex_);

    std::ifstream inputFile(pendingFile_);

    if (!inputFile) {
        return true;
    }

    std::vector<std::string> pending;
    std::string message;

    while (std::getline(inputFile, message)) {
        pending.push_back(message);
    }

    if (inputFile.bad()) {
        return false;
    }

    inputFile.close();

    std::ofstream outputFile(storageFile_, std::ios::app);

    if (!outputFile) {
        return false;
    }

    for (const auto& item : pending) {
        outputFile << item << '\n';
    }

    outputFile.flush();

    if (!outputFile) {
        return false;
    }

    outputFile.close();

    if (!outputFile) {
        return false;
    }

    return std::remove(pendingFile_.c_str()) == 0;
}
