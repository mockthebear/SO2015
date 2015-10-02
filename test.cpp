#include "comunication.hpp"
#include <iostream>

int main(){
    MessageQueue<127> SendMessage(QUEUE_SEND,1337,0666);
    MessageQueue<127> RecvMessage(QUEUE_RECV,1338,0666);

    if (!SendMessage.isWorking() or !RecvMessage.isWorking()){
        std::cout << "Not working :c\n";
        return 1;
    }
    SendMessage.Send("Hi!\n");
    std::cout << "Got: " << RecvMessage.Receive() << "\n";
    getchar();
    return 0;
}
