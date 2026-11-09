#include "message_spool.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

int main() {
    const std::string filename = "test_messages.txt";
    const std::string journal = filename + ".journal";

    std::remove(filename.c_str());
    std::remove(journal.c_str());

    {
        MessageSpool spool(filename);

        assert(spool.saveMessage("Message one"));
        assert(spool.saveMessage("Message two"));
        assert(spool.saveMessage("Message three"));

        const auto messages = spool.loadMessages();

        assert(messages.size() == 3);
        assert(messages[0] == "Message one");
        assert(messages[1] == "Message two");
        assert(messages[2] == "Message three");

        std::uint64_t id1 = 0;
        std::uint64_t id2 = 0;
        std::uint64_t id3 = 0;

        assert(spool.enqueueMessage("Pending one", id1));
        assert(spool.enqueueMessage("Pending two", id2));
        assert(spool.enqueueMessage("Pending three", id3));

        assert(id1 != id2);
        assert(id2 != id3);
        assert(spool.pendingCount() == 3);

        assert(spool.acknowledgeMessage(id2));
        assert(spool.pendingCount() == 2);
    }

    // Simulate restarting the application.
    {
        MessageSpool recovered(filename);

        const auto pending = recovered.loadPendingMessages();

        assert(pending.size() == 2);
        assert(pending[0].content == "Pending one");
        assert(pending[1].content == "Pending three");

        assert(recovered.acknowledgeMessage(pending[0].id));
        assert(recovered.acknowledgeMessage(pending[1].id));

        assert(recovered.pendingCount() == 0);
    }

    std::remove(filename.c_str());
    std::remove(journal.c_str());

    std::cout << "Persistence and recovery tests passed.\n";

    return 0;
}
