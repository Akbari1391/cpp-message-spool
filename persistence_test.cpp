#include "message_spool.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

int main() {
    const std::string filename = "test_messages.txt";

    std::remove(filename.c_str());

    MessageSpool spool(filename);

    assert(spool.saveMessage("Message one"));
    assert(spool.saveMessage("Message two"));
    assert(spool.saveMessage("Message three"));

    const std::vector<std::string> messages = spool.loadMessages();

    assert(messages.size() == 3);
    assert(messages[0] == "Message one");
    assert(messages[1] == "Message two");
    assert(messages[2] == "Message three");

    std::remove(filename.c_str());

    std::cout << "Persistence test passed." << std::endl;

    return 0;
}