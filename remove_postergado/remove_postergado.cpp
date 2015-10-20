#include "../comunication.hpp"
#include <iostream>
#include <string.h>

int main(int argc, char *argv[]){
    if (argc < 2){
        std::cout << "Usage:\nremove_postergado <id>\nremove_postergado 2";
        return 0;
    }
    //Programa de shutdown
    MessageQueue SendMessage(QUEUE_SEND,0x1338, 0x124);
    MessageQueue RecvMessage(QUEUE_RECV,0x1337, 0x124);
    if (!SendMessage.isWorking() or !RecvMessage.isWorking()){
        std::cout << "Not working :c\n";
        return 1;
    }
    int value;
    sscanf(argv[2],"%d",&value);
    char msg[100];
    msg[0] = 1; //Command
    msg[1] = 'r'; //remove
    msg[2] = (value+1); //id
    msg[3] = 0; //end
    if (!SendMessage.Send(msg,strlen(msg)+1)){
        std::cout << "[Error] not sent.\n";
    }else{
		std::string Message = RecvMessage.Receive(true);
        std::cout << "[Info]" << Message << "]\n";
    }
    return 0;
}
