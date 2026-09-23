#ifndef MESSAGE_SPOOL_H
#define MESSAGE_SPOOL_H

#include <string>
#include <vector>

class MessageSpool {
public:
    explicit MessageSpool(const std::string& storageFile);

    bool saveMessage(const std::string& message);
    std::vector<std::string> loadMessages() const;

private:
    std::string storageFile_;
};

#endif