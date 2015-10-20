#include "../comunication.hpp"
#include <iostream>
#include <string.h>

int main(){
    //Programa de shutdown
    MessageQueue SendMessage(QUEUE_SEND,0x1338, 0x124);
    MessageQueue RecvMessage(QUEUE_RECV,0x1337, 0x124);
    if (!SendMessage.isWorking() or !RecvMessage.isWorking()){
        std::cout << "Not working :c\n";
        return 1;
    }
    char msg[10];
    msg[0] = 1; //Command
    msg[1] = 's'; //Shutdown
    msg[3] = 0; //end
    if (!SendMessage.Send(msg,strlen(msg)+1)){
        std::cout << "not sent.\n";
    }else{
        std::cout << "sent.\n";
		std::string Message = RecvMessage.Receive(true);
        std::cout << "[Info]" << Message << "]\n";
    }
	//Better not close because CRASHES!!!
	/*
    SendMessage.Close();
    RecvMessage.Close();
	actually the main reason its because you are closing the
	message queue.
	should be open because the server are still using
	*/

    return 0;
}
