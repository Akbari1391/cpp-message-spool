#include <cassert>
#include <iostream>
#include <queue>
#include <string>

void testQueueFIFO() {
    std::queue<std::string> queue;

    queue.push("Message one");
    queue.push("Message two");
    queue.push("Message three");

    assert(queue.front() == "Message one");
    queue.pop();

    assert(queue.front() == "Message two");
    queue.pop();

    assert(queue.front() == "Message three");
    queue.pop();

    assert(queue.empty());
}

int main() {
    testQueueFIFO();

    std::cout << "All tests passed." << std::endl;

    return 0;
}