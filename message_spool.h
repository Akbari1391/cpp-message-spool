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

    bool saveMessage(const std::string& message);
    std::vector<std::string> loadMessages() const;

    bool enqueueMessage(const std::string& message,
                        std::uint64_t& messageId);

    // Returns false if the journal cannot be read or is corrupted.
    bool loadPendingMessages(
        std::vector<MessageRecord>& messages) const;

    bool acknowledgeMessage(std::uint64_t messageId);

    // Returns false on journal errors.
    bool pendingCount(std::size_t& count) const;

private:
    std::string storageFile_;
    std::string journalFile_;
    mutable std::mutex fileMutex_;
};

#endif
