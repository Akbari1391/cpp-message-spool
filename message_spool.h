#ifndef MESSAGE_SPOOL_H
#define MESSAGE_SPOOL_H

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

struct MessageRecord {
    std::uint64_t id;
    std::string content;
};

class MessageSpool {
public:
    explicit MessageSpool(const std::string& storageFile);

    // Existing functionality
    bool saveMessage(const std::string& message);
    std::vector<std::string> loadMessages() const;

    // Reliable message processing
    bool enqueueMessage(const std::string& message,
                        std::uint64_t& messageId);

    std::vector<MessageRecord> loadPendingMessages() const;

    bool acknowledgeMessage(std::uint64_t messageId);

    std::size_t pendingCount() const;

private:
    std::string storageFile_;
    std::string journalFile_;
    mutable std::mutex fileMutex_;
};

#endif
