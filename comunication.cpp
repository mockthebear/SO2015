#include "comunication.hpp"
#include <iostream>

MessageQueue::MessageQueue(QueueType type_arg,int key,int flags){
    qKey            =   key;
    qFlags          =   flags;
    type            =   type_arg;
    Owner           =   false;
    Buffer.mtype    =   1;
    Created         =   false;
    //Solicita fila de mensagens com key e flags definidas
    if (qFlags|IPC_CREAT){
        Owner = true;
    }
    MessageQueueId  = msgget(qKey, qFlags);
    if (MessageQueueId < 0){
        //Falha
        std::cout << MessageQueueId << "]\n";
        Working = false;
    }else{
        Created = true;
        Working = true;
    }

}
MessageQueue::~MessageQueue(){}

void MessageQueue::Close(){
    if (Created){
        if (msgctl(MessageQueueId, IPC_RMID, NULL) == 0){
            Created = false;
        }else{
            std::cout << "[Error] Cannot delete!\n";
        }
    }
}

//Polimorfismo
bool MessageQueue::Send(std::string str){
    const char *c = str.c_str();
    return Send(c,str.size()+1);
}
bool MessageQueue::Send(const char *message,int size){
    return Send((char*)message,size);
}

bool MessageQueue::Send(char *message,int size){
    if (!isSender()){
        std::cout << "[Error] i am not sender!\n";
        return false;
    }
    if (size > MSG_SIZE){
        std::cout << "[Error] message too big\n";
        return false;
    }
    //Copy
    for (int i=0;i<size;i++){
        Buffer.mtext[i] = message[i];
    }
    if (msgsnd(MessageQueueId, &Buffer, size, 0) < 0) {
       return false;
    }
    return true;
}

std::string MessageQueue::Receive(bool wait){
    std::string msg = "";
    Receive(msg,wait);
    return msg;
}

bool MessageQueue::Receive(std::string &str,bool wait){
    char message[MSG_SIZE];
    if (!Receive(message,wait)){
        return false;
    }
    std::string data = message;
    str = data;
    return true;
}

bool MessageQueue::Receive(char *message,bool wait){
    if (!isWorking()){
        std::cout << "[Error] not working!\n";
        return false;
    }
    if (!isReceiver()){
        std::cout << "[Error] i am not receiver! ["<<type<<"]\n";
        return false;
    }
    int readSize=-1;
    readSize = msgrcv(MessageQueueId, &Buffer, MSG_SIZE, 1,(wait == true ? 0 : IPC_NOWAIT));
    if (readSize < 0){
        return false;
    }
    LastReadSize = readSize;
    for (int i=0;i<MSG_SIZE+1;i++){
        message[i] = Buffer.mtext[i];
    }
    return true;
}


