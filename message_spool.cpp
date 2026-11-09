#include "message_spool.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string encodeMessage(const std::string& message) {
    std::ostringstream output;

    for (unsigned char ch : message) {
        output << std::hex << std::setfill('0')
               << std::setw(2) << static_cast<unsigned int>(ch);
    }

    return output.str();
}

bool decodeMessage(const std::string& encoded, std::string& message) {
    if (encoded.size() % 2 != 0) {
        return false;
    }

    message.clear();

    for (std::size_t i = 0; i < encoded.size(); i += 2) {
        unsigned int value = 0;
        std::istringstream input(encoded.substr(i, 2));

        input >> std::hex >> value;

        if (input.fail() || !input.eof()) {
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
        char operation;
        std::uint64_t id;

        if (!(record >> operation >> id)) {
            return false;
        }

        if (operation == 'E') {
            std::string encoded;
            std::string message;

            if (!(record >> encoded)) {
                return false;
            }

            if (!decodeMessage(encoded, message)) {
                return false;
            }

            pending[id] = message;
        } else if (operation == 'A') {
            pending.erase(id);
        } else {
            return false;
        }

        maxId = std::max(maxId, id);
    }

    return !input.bad();
}

bool appendJournal(
    const std::string& filename,
    const std::string& record) {

    std::ofstream output(filename, std::ios::app | std::ios::binary);

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

    std::map<std::uint64_t, std::string> pending;
    std::uint64_t maxId = 0;

    {
        std::ifstream check(journalFile_);

        if (check && !readJournal(journalFile_, pending, maxId)) {
            return false;
        }
    }

    if (maxId == UINT64_MAX) {
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

std::vector<MessageRecord> MessageSpool::loadPendingMessages() const {
    std::lock_guard<std::mutex> lock(fileMutex_);

    std::vector<MessageRecord> result;
    std::map<std::uint64_t, std::string> pending;
    std::uint64_t maxId = 0;

    std::ifstream check(journalFile_);

    if (!check) {
        return result;
    }

    check.close();

    if (!readJournal(journalFile_, pending, maxId)) {
        return result;
    }

    for (const auto& entry : pending) {
        result.push_back({entry.first, entry.second});
    }

    return result;
}

bool MessageSpool::acknowledgeMessage(std::uint64_t messageId) {
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

std::size_t MessageSpool::pendingCount() const {
    return loadPendingMessages().size();
}
