#include "comunication.hpp"


template<size_t Size> MessageQueue<Size>::MessageQueue(QueueType type_arg,int key,int flags){
    qKey            =   key;
    qFlags          =   flags;
    type            =   type_arg;
    //Solicita fila de mensagens com key e flags definidas
    //MessageQueueId  = msgget(qKey, qFlags);
    if (MessageQueueId < 0){
        //Falha
        Working = false;
    }else{
        Working = true;
    }

}
//Polimorfismo
template<size_t Size> bool MessageQueue<Size>::Send(std::string str){
    const char *c = str.c_str();
    return Send(c,str.size()+1);
}
template<size_t Size> bool MessageQueue<Size>::Send(const char *message,int size){
    return Send((char*)message,size);
}

template<size_t Size> bool MessageQueue<Size>::Send(char *message,int size){
    if (!isSender()){
        return false;
    }
    if (size > Size){
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

template<size_t Size> std::string MessageQueue<Size>::Receive(bool wait){
    std::string msg = "[null]";
    Receive(msg,wait);
    return msg;
}

template<size_t Size> bool MessageQueue<Size>::Receive(std::string &str,bool wait){
    char *message[Size];
    if (!Receive(message,wait)){
        return false;
    }
    str = message;
    return true;
}

template<size_t Size> bool MessageQueue<Size>::Receive(char *message,bool wait){
    if (!isReceiver()){
        return false;
    }
    int readSize=-1;
    if (readSize = msgrcv(MessageQueueId, &Buffer, Size, 1, (wait ? 0 : IPC_NOWAIT) ) < 0){
        return false;
    }
    LastReadSize = readSize;
    for (int i=0;i<readSize;i++){
        Buffer.mtext[i] = message[i];
    }
    return true;
}


