#include "message_spool.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define CHECK(condition)                                      \
    do {                                                      \
        if (!(condition)) {                                   \
            std::cerr << "Test failed: " #condition           \
                      << " at line " << __LINE__ << '\n';      \
            return 1;                                         \
        }                                                     \
    } while (false)

int main() {
    const std::string filename = "test_messages.txt";
    const std::string journal = filename + ".journal";

    std::remove(filename.c_str());
    std::remove(journal.c_str());

    // Test 1: First run without an existing journal.
    {
        MessageSpool spool(filename);
        std::vector<MessageRecord> pending;
        std::size_t count = 999;

        CHECK(spool.loadPendingMessages(pending));
        CHECK(pending.empty());
        CHECK(spool.pendingCount(count));
        CHECK(count == 0);
    }

    // Test 2: Save and load completed messages.
    {
        MessageSpool spool(filename);

        CHECK(spool.saveMessage("Message one"));
        CHECK(spool.saveMessage("Message two"));
        CHECK(spool.saveMessage("Message three"));

        const auto messages = spool.loadMessages();

        CHECK(messages.size() == 3);
        CHECK(messages[0] == "Message one");
        CHECK(messages[1] == "Message two");
        CHECK(messages[2] == "Message three");
    }

    // Test 3: Enqueue and acknowledge messages.
    std::uint64_t id1 = 0;
    std::uint64_t id2 = 0;
    std::uint64_t id3 = 0;
    std::uint64_t id4 = 0;

    {
        MessageSpool spool(filename);

        CHECK(spool.enqueueMessage("Pending one", id1));
        CHECK(spool.enqueueMessage("Pending two", id2));
        CHECK(spool.enqueueMessage("Pending three", id3));

        CHECK(id1 == 1);
        CHECK(id2 == 2);
        CHECK(id3 == 3);

        CHECK(spool.acknowledgeMessage(id2));

        std::size_t count = 0;
        CHECK(spool.pendingCount(count));
        CHECK(count == 2);

        // IDs must not be reused after acknowledgements.
        CHECK(spool.enqueueMessage("Pending four", id4));
        CHECK(id4 == 4);
    }

    // Test 4: Recover pending messages after restart.
    {
        MessageSpool recovered(filename);
        std::vector<MessageRecord> pending;

        CHECK(recovered.loadPendingMessages(pending));
        CHECK(pending.size() == 3);

        CHECK(pending[0].id == id1);
        CHECK(pending[0].content == "Pending one");

        CHECK(pending[1].id == id3);
        CHECK(pending[1].content == "Pending three");

        CHECK(pending[2].id == id4);
        CHECK(pending[2].content == "Pending four");

        CHECK(recovered.acknowledgeMessage(id1));
        CHECK(recovered.acknowledgeMessage(id3));
        CHECK(recovered.acknowledgeMessage(id4));

        std::size_t count = 999;
        CHECK(recovered.pendingCount(count));
        CHECK(count == 0);
    }

    // Test 5: IDs must remain unique after all messages are acknowledged.
    {
        MessageSpool spool(filename);
        std::uint64_t newId = 0;

        CHECK(spool.enqueueMessage("After restart", newId));
        CHECK(newId == 5);
        CHECK(spool.acknowledgeMessage(newId));
    }

    // Test 6: Detect journal corruption.
    {
        std::ofstream output(journal, std::ios::app);

        CHECK(static_cast<bool>(output));

        output << "INVALID JOURNAL RECORD\n";
        output.close();

        CHECK(static_cast<bool>(output));

        MessageSpool spool(filename);
        std::vector<MessageRecord> pending;

        CHECK(!spool.loadPendingMessages(pending));

        std::size_t count = 0;
        CHECK(!spool.pendingCount(count));

        std::uint64_t newId = 0;
        CHECK(!spool.enqueueMessage("New message", newId));
    }

    std::remove(filename.c_str());
    std::remove(journal.c_str());

    std::cout << "All persistence and recovery tests passed.\n";

    return 0;
}
