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
//Envia para o programa prar executar alguma coisa
typedef struct{
    long    mtype;
    char    mtext[MSGSZ];
} message_buf;

int main(int argc,char *argv[])
{
    //./execproc programa[args]
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
    //Sending message type
    strcpy(sbuf.mtext, argv[1]);
    sbuf.mtext[0] = 2;
    if (msgsnd(msqid, &sbuf, ( strlen(sbuf.mtext) + 1), 0) < 0) {
       printf("Error!!\n");
    }
    //Sending file name
    strcpy(sbuf.mtext, argv[1]);
    if (msgsnd(msqid, &sbuf, ( strlen(sbuf.mtext) + 1), 0) < 0) {
       printf("Error!\n");
    }

    //Send argument count
    int argN = (argc-1);
    sprintf(sbuf.mtext,"%d", argN);
    if (msgsnd(msqid, &sbuf, ( strlen(sbuf.mtext) + 1), 0) < 0) {
       printf("Error!!\n");
    }
    //Send argument 0
    sprintf(sbuf.mtext,"%s", argv[0]);
    if (msgsnd(msqid, &sbuf, ( strlen(sbuf.mtext) + 1), 0) < 0) {
       printf("Error!!\n");
    }
    //Sending other arguments!
    for (int i=0;i<argN-1;i++){
        //Enviar N parametros
        sprintf(sbuf.mtext,"%s", argv[2+i]);
        if (msgsnd(msqid, &sbuf, ( strlen(sbuf.mtext) + 1), 0) < 0) {
           printf("Error!!\n");
        }
    }
    //Gettingg the output
    if (msgrcv(msqidrecv, &rbuf, MSGSZ, 1, 0) < 0) {
        printf("Treta\n");
        return 1;
    }
    printf("Output: %s\n",rbuf.mtext);
    return 0;
}
