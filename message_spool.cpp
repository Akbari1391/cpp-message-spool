#include "message_spool.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string encodeMessage(const std::string& message) {
    std::ostringstream output;

    for (unsigned char ch : message) {
        output << std::hex << std::setfill('0')
               << std::setw(2)
               << static_cast<unsigned int>(ch);
    }

    return output.str();
}

bool decodeMessage(const std::string& encoded,
                   std::string& message) {
    if (encoded.size() % 2 != 0) {
        return false;
    }

    message.clear();

    for (std::size_t i = 0; i < encoded.size(); i += 2) {
        unsigned int value = 0;
        std::istringstream input(encoded.substr(i, 2));

        input >> std::hex >> value;

        if (input.fail() || !input.eof() || value > 255) {
            return false;
        }

        message.push_back(static_cast<char>(value));
    }

    return true;
}

bool readJournal(
    const std::string& filename,
    std::map<std::uint64_t, std::string>& pending,
    std::uint64_t& maxId) {

    std::ifstream input(filename, std::ios::binary);

    if (!input) {
        return false;
    }

    std::string line;

    while (std::getline(input, line)) {
        std::istringstream record(line);
        char operation = '\0';
        std::uint64_t id = 0;

        if (!(record >> operation >> id) || id == 0) {
            return false;
        }

        std::string extra;

        if (operation == 'E') {
            std::string encoded;
            std::string message;

            if (!(record >> encoded) || (record >> extra)) {
                return false;
            }

            if (!decodeMessage(encoded, message)) {
                return false;
            }

            if (id <= maxId || pending.count(id) != 0) {
                return false;
            }

            pending[id] = message;
            maxId = id;

        } else if (operation == 'A') {
            if (record >> extra) {
                return false;
            }

            if (pending.erase(id) != 1) {
                return false;
            }

        } else {
            return false;
        }
    }

    return !input.bad();
}

bool journalExists(const std::string& filename,
                   bool& exists) {
    std::error_code error;

    exists = std::filesystem::exists(filename, error);

    return !error;
}

bool appendJournal(const std::string& filename,
                   const std::string& record) {
    std::ofstream output(
        filename,
        std::ios::app | std::ios::binary
    );

    if (!output) {
        return false;
    }

    output << record << '\n';
    output.flush();

    return static_cast<bool>(output);
}

} // namespace

MessageSpool::MessageSpool(const std::string& storageFile)
    : storageFile_(storageFile),
      journalFile_(storageFile + ".journal") {
}

bool MessageSpool::saveMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(fileMutex_);

    std::ofstream output(storageFile_, std::ios::app);

    if (!output) {
        return false;
    }

    output << message << '\n';
    output.flush();

    return static_cast<bool>(output);
}

std::vector<std::string> MessageSpool::loadMessages() const {
    std::lock_guard<std::mutex> lock(fileMutex_);

    std::vector<std::string> messages;
    std::ifstream input(storageFile_);
    std::string message;

    while (std::getline(input, message)) {
        messages.push_back(message);
    }

    return messages;
}

bool MessageSpool::enqueueMessage(
    const std::string& message,
    std::uint64_t& messageId) {

    std::lock_guard<std::mutex> lock(fileMutex_);

    if (message.empty()) {
        return false;
    }

    std::map<std::uint64_t, std::string> pending;
    std::uint64_t maxId = 0;
    bool exists = false;

    if (!journalExists(journalFile_, exists)) {
        return false;
    }

    if (exists &&
        !readJournal(journalFile_, pending, maxId)) {
        return false;
    }

    if (maxId == std::numeric_limits<std::uint64_t>::max()) {
        return false;
    }

    const std::uint64_t newId = maxId + 1;

    const std::string record =
        "E " + std::to_string(newId) + " " +
        encodeMessage(message);

    if (!appendJournal(journalFile_, record)) {
        return false;
    }

    messageId = newId;
    return true;
}

bool MessageSpool::loadPendingMessages(
    std::vector<MessageRecord>& messages) const {

    std::lock_guard<std::mutex> lock(fileMutex_);

    messages.clear();

    bool exists = false;

    if (!journalExists(journalFile_, exists)) {
        return false;
    }

    // A missing journal means a fresh installation.
    if (!exists) {
        return true;
    }

    std::map<std::uint64_t, std::string> pending;
    std::uint64_t maxId = 0;

    if (!readJournal(journalFile_, pending, maxId)) {
        return false;
    }

    for (const auto& entry : pending) {
        messages.push_back({entry.first, entry.second});
    }

    return true;
}

bool MessageSpool::acknowledgeMessage(
    std::uint64_t messageId) {

    std::lock_guard<std::mutex> lock(fileMutex_);

    std::map<std::uint64_t, std::string> pending;
    std::uint64_t maxId = 0;

    if (!readJournal(journalFile_, pending, maxId)) {
        return false;
    }

    if (pending.find(messageId) == pending.end()) {
        return false;
    }

    return appendJournal(
        journalFile_,
        "A " + std::to_string(messageId)
    );
}

bool MessageSpool::pendingCount(
    std::size_t& count) const {

    std::vector<MessageRecord> messages;

    if (!loadPendingMessages(messages)) {
        return false;
    }

    count = messages.size();
    return true;
}
