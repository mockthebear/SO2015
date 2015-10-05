//batata
#ifndef MSGQUEUEMOCK
#define MSGQUEUEMOCK
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define MSG_SIZE 128


enum QueueType{
    QUEUE_SEND=0b001,
    QUEUE_RECV=0b010,
    QUEUE_BOTH=0b011,
};

class MessageQueue{
    public:
        MessageQueue(QueueType type,int key,int flags);
        ~MessageQueue();
        void Close();
        bool isWorking()    {return Working;};
        bool isOwner()    {return Owner;};
        /*
            Aqui ele faz a operação AND binaria em type e a constante
            Se type for 1 e ele quiser verificar se é do tipo send:
            (type)   (cte)
            0001 and 0001 = 0001 (true)
            se for receive:
            0010 and 0001 = 0000 (false)
            e se for booth
            0011 and 0001 = 0001 (true)
        */
        bool isSender()     {return isWorking() && (type | QUEUE_SEND);};
        bool isReceiver()   {return isWorking() && (type | QUEUE_RECV);};

        bool Send(const char *message,int size);
        bool Send(char *message,int size);
        bool Send(std::string str);

        bool Receive(char *message,bool wait=true);
        bool Receive(std::string &str,bool wait=true);
        std::string Receive(bool wait=true);
        int GetReadSize(){return LastReadSize;};

    private:
        int MessageQueueId;
        int qKey;
        int qFlags;
        QueueType type; // < P***
        bool Working,Owner,Created;
        int LastReadSize;
        struct message_buf{
            long    mtype;
            char    mtext[MSG_SIZE];
        } Buffer;
};

#endif // MSGQUEUEMOCK
