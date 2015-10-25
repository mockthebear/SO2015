#include "../comunication.hpp"
#include <iostream>
#include <string.h>

int main(int argc, char *argv[]){
    if (argc < 4){
        std::cout << "Usage:\nexecuta_postergado <hh:mm> <amount> <file>\nexecuta_postergado 00:01 10 hello\n";
        return 0;
    }

    //Programa de shutdown
    MessageQueue SendMessage(QUEUE_SEND,0x1338, 0x124);
    MessageQueue RecvMessage(QUEUE_RECV,0x1337, 0x124);
    if (!SendMessage.isWorking() or !RecvMessage.isWorking()){
        std::cout << "Not working!\n";
        return 1;
    }
    
    int HH, MM, amount;
    
    sscanf(argv[1],"%d:%d",&HH,&MM);
    sscanf(argv[2],"%d",&amount);
    
    char msg[100];
    
    msg[0] = 1; //Command
    msg[1] = 'e'; //Shutdown
    msg[2] = (HH+1); //hora
    msg[3] = (MM+1); //min
    msg[4] = (amount+1); //vezes
    strcpy(&msg[5],argv[3]);
	msg[6+strlen(argv[3])] = 0;
    
    if (!SendMessage.Send(msg,strlen(msg)+1)){
        std::cout << "not sent.\n";
    }else{
        std::cout << "<"<<(int)msg[2]<<":"<<(int)msg[3]<<"> "<< (int)msg[4]<<" >= " << (&msg[5]) << "\n";
		std::string Message = RecvMessage.Receive(true);
        std::cout << "[Info]" << Message << "]\n";
    }
    return 0;
}
