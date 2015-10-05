#include "comunication.hpp"
#include <iostream>
#include <string.h>

int main(){
    std::cout << "Type S to sender and R to receiver. Note that reciver will be on loop and the owner.\n";
    char c = getchar();
    char msg[128];
    if (c == 'S' or c =='s'){
        /*
            Aqui é o "programa que envia mensagem
        */
        MessageQueue SendMessage(QUEUE_SEND,0x1338, 0x124);
        MessageQueue RecvMessage(QUEUE_RECV,0x1337, 0x124);

        if (!SendMessage.isWorking() or !RecvMessage.isWorking()){
            std::cout << "Not working :c\n";
            return 1;
        }

        //Notar que os IDS foram invertidos.
        std::cout << "Envie a mensagem\n";
        getchar();
        scanf("%[^\n]",msg);
        if (!SendMessage.Send(msg,strlen(msg)+1)){
            std::cout << "not sent.\n";
        }
        std::string Message = RecvMessage.Receive(true); //Se trocar false p true, ele n fica não blockante
        std::cout << "Recebi de volta isso [" << Message << "]{"<<Message.size()<<"}\n";
    }else{
        //Receiver!
        MessageQueue SendMessage(QUEUE_SEND,0x1337, IPC_CREAT | 0x1FF); //CREATE and GET.
        MessageQueue RecvMessage(QUEUE_RECV,0x1338, IPC_CREAT | 0x1FF); //CREATE and GET.
        if (!SendMessage.isWorking() or !RecvMessage.isWorking()){
            std::cout << "Not working :c\n";
            return 1;
        }
        while (1){
            std::string Message = RecvMessage.Receive(true);
            std::cout << "Got: [" << Message << "]{"<<Message.size()<<"}\n";
            SendMessage.Send(Message + " foi oq vc mandou?");
            if (Message == "bye"){
                break;
            }
        }

        SendMessage.Close();
        RecvMessage.Close();




    }
    getchar();
    return 0;
}
