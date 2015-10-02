/*
    Matheus Braga almeida 120038412
    matheus.mtb7@gmail.com

*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>

#define MSGSZ     128
//Kill a selected process
typedef struct{
    long    mtype;
    char    mtext[MSGSZ];
} message_buf;

int main(int argc,char *argv[])
{
    //./execproc programa
    if (argc <= 1){
        return 1;
    }
    int msqid,msqidrecv;
    int msgflg = 0666;
    key_t key,key_recv;
    size_t buf_length;
    message_buf sbuf,rbuf;
    key = 1337;
    key_recv = 1338;
    sbuf.mtype = 1;
    if ((msqid = msgget(key, msgflg )) < 0) {
        return 1;
    }
    if ((msqidrecv = msgget(key_recv, msgflg )) < 0) {
        return 1;
    }

    //Sending the message ID
    strcpy(sbuf.mtext, "??");
    sbuf.mtext[0] = 3;
    if (msgsnd(msqid, &sbuf, (strlen(sbuf.mtext) + 1), 0) < 0) {
       printf("Error!!\n");
    }
    //Sending the file name
    strcpy(sbuf.mtext, argv[1]);
    if (msgsnd(msqid, &sbuf, (strlen(sbuf.mtext) + 1), 0) < 0) {
       printf("Error!\n");
    }
    //Get the output
    msgrcv(msqidrecv, &rbuf, MSGSZ, 1, 0);
    printf("Output: %s\n",rbuf.mtext);

    return 0;
}
