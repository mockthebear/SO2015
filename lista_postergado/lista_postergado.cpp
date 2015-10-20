#include "../comunication.hpp"
#include <iostream>
#include <string.h>

int main(int argc, char *argv[]){
    //Programa de shutdown
    MessageQueue SendMessage(QUEUE_SEND,0x1338, 0x124);
    MessageQueue RecvMessage(QUEUE_RECV,0x1337, 0x124);
    if (!SendMessage.isWorking() or !RecvMessage.isWorking()){
        std::cout << "Not working :c\n";
        return 1;
    }
    char msg[100];
    msg[0] = 1; //Command
    msg[1] = 'l'; //remove
    msg[2] = 0; //end
    if (!SendMessage.Send(msg,strlen(msg)+1)){
        std::cout << "[Error] not sent.\n";
    }else{
        while (1){
            std::string Message = RecvMessage.Receive(true);
            if (Message == "end"){
                break;
            }
            std::cout << Message;
        }
    }
    return 0;
}
