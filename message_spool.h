#ifndef MESSAGE_SPOOL_H
#define MESSAGE_SPOOL_H

#include <string>
#include <vector>
#include <mutex>

class MessageSpool {
public:
    explicit MessageSpool(const std::string& storageFile);

    bool saveMessage(const std::string& message);
    std::vector<std::string> loadMessages() const;

    bool savePendingMessage(const std::string& message);
    std::vector<std::string> loadPendingMessages() const;
    bool clearPendingMessages();

private:
    std::string storageFile_;
    std::string pendingFile_;
    mutable std::mutex fileMutex_;
};

#endif
