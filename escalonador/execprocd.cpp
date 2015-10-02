/*
    Matheus Braga almeida 120038412
    matheus.mtb7@gmail.com

*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#include <iostream>
#include <stdlib.h>

#define MSGSZ     128
#include <sys/time.h>
#include <unistd.h>

#include <vector>



typedef struct{
    long    mtype;
    char    mtext[MSGSZ];
} message_buf;

int main(int argc,char *argv[])
{
    int msqid,msqid_send;
    int msgflg = IPC_CREAT | 0666;

    struct timeval Quantum, auxT;

    key_t key,keysend;
    message_buf sbuf,rbuf;
    size_t buf_length;
    pid_t pid;

    key = 1337;
    keysend = 1338;

    sbuf.mtype = 1;
    bool showOut=false;

    if (argc >= 2){
        showOut = true;
    }

    std::vector<int> Processos;
    int LastOnQuantum = -1;
    gettimeofday(&Quantum, NULL);

    if ((msqid = msgget(key, msgflg )) < 0) {
        return 1;
    }

    if ((msqid_send = msgget(keysend, msgflg )) < 0) {
        return 1;
    }

    while (1){
        gettimeofday(&auxT, NULL);
        unsigned int seconds  = auxT.tv_sec  - Quantum.tv_sec;
        unsigned int useconds = auxT.tv_usec - Quantum.tv_usec;
        unsigned int mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
        if (mtime >= 2000){
            if (Processos.size() > 0){
                //Select a random process.
                //In case of the new process is the same as that still are
                //in the cpu, keep it in!
                int ID = rand()%Processos.size();
                if (LastOnQuantum != -1 && LastOnQuantum != ID){
                    int status;
                    pid_t result = waitpid(LastOnQuantum, &status, WNOHANG);
                    if (result != 0){
                        //Exited
                        if (showOut)
                            std::cout << "Process " << LastOnQuantum << " has finished and returned " << status << "\n";
                        //Realloc and re select an random process!
                        std::vector<int> Processos_aux;
                        for(int i=0;i<Processos.size();i++){
                           if (Processos[i] != LastOnQuantum){
                                Processos_aux.emplace_back(Processos[i]);
                           }
                        }
                        Processos = Processos_aux;
                        if (Processos.size() > 0)
                            int ID = rand()%Processos.size();

                    }else{
                        if (LastOnQuantum != Processos[ID]){
                            if (showOut)
                                std::cout << "[SIGSTOP] > " << LastOnQuantum <<"\n";
                            kill(LastOnQuantum, SIGSTOP);
                            LastOnQuantum = -1;
                        }
                    }

                }
                if (Processos.size() > 0 and Processos[ID] and LastOnQuantum != Processos[ID]){
                    LastOnQuantum = Processos[ID];
                    if (showOut)
                        std::cout << "[SIGCONT] > " << LastOnQuantum <<"\n";
                    kill(LastOnQuantum, SIGCONT);
                }

            }
            gettimeofday(&Quantum, NULL);
        }
        //Message receive stuff
        if (msgrcv(msqid, &rbuf, MSGSZ, 1, IPC_NOWAIT) < 0) {

        }else{
            //2 means to start a process
            if (rbuf.mtext[0] == 2){
                if (showOut)
                    std::cout << "Starting process: ";
                msgrcv(msqid, &rbuf, MSGSZ, 1,0);
                char buffer[50];
                strcpy(buffer,rbuf.mtext);
                msgrcv(msqid, &rbuf, MSGSZ, 1,0);
                int paramSize = atoi(rbuf.mtext);
                if (showOut)
                    std::cout << buffer << " [params: "<< paramSize<<" ]\n";
                //File exist?

                char **params = new char*[paramSize];
                for (int i=0;i<paramSize;i++){
                    params[i] = new char[50];
                    msgrcv(msqid, &rbuf, MSGSZ, 1,0);
                    strcpy(params[i],rbuf.mtext);
                    if (showOut)
                        std::cout << "P["<<i<<"] '" << params[i] <<"'\n";
                }


                bool fail = false;
                FILE*fp = fopen(buffer,"rb");
                if (fp != NULL){
                    fclose(fp);
                    pid = fork();
                    if (pid == -1) {
                        if (showOut)
                            std::cout << "Tretas aconteceram!\n";
                        return 1;
                    }else if (pid == 0) {
                        //Execução
                        if ( execv(buffer, params) < 0 ){
                            if (showOut)
                                std::cout << "File caanot be executed!\n";
                            fail = true;
                        }
                    }
                }else{
                    fail = true;
                }
                sbuf.mtype = 1;
                //Esperar agora confirmação de que recebeu
                if (not fail){
                    kill(pid, SIGSTOP);
                    Processos.emplace_back(pid);
                    if (showOut)
                        std::cout << "Created process "<<pid << "\n";
                    sprintf(sbuf.mtext,"Created pid %d",pid);
                }else{
                    char errstr[] = "Cannot execute. Has something wrong with the file.";
                    strcpy(sbuf.mtext, errstr);
                }
                for (int i=0;i<paramSize;i++){
                     delete params[i];
                }
                delete params;
                //Enviar mensagem informando se executou ou não
                buf_length = strlen(sbuf.mtext) + 1 ;
                if (msgsnd(msqid_send, &sbuf, buf_length, 0) < 0) {
                    if (showOut)
                        std::cout <<"Error on sending the message!!\n";
                }
            }else if (rbuf.mtext[0] == 1){
                //close!
                break;
            }else if (rbuf.mtext[0] == 3){
                msgrcv(msqid, &rbuf, MSGSZ, 1,0);
                int pid = atoi(rbuf.mtext);
                int index = -1;
                if (showOut)
                    std::cout << "Looking for pid "<<pid<<"\n";
                bool find = false;
                for (int i=0;i<Processos.size();i++){
                    if (Processos[i] == pid){
                        index = i;
                        find = true;
                        break;
                    }
                }
                if (not find){
                    char errstr[] = "Process not found =/\n";
                    strcpy(sbuf.mtext, errstr);
                }else{
                    if (showOut)
                        std::cout << "[SIGQUIT] > " << LastOnQuantum <<"\n";
                    kill(Processos[index],SIGQUIT);
                    std::vector<int> Processos_aux;
                    for(int i=0;i<Processos.size();i++){
                        if (Processos[i] != Processos[index]){
                            Processos_aux.emplace_back(Processos[i]);
                        }
                    }
                    Processos = Processos_aux;
                    if (LastOnQuantum == Processos[index]){
                        LastOnQuantum = -1;
                    }
                    char errstr[] = "Process is closed now!\n";
                    strcpy(sbuf.mtext, errstr);
                }
                sbuf.mtype = 1;
                buf_length = strlen(sbuf.mtext) + 1 ;
                if (msgsnd(msqid_send, &sbuf, buf_length, 0) < 0) {
                    if (showOut)
                        std::cout << "Error on sending the message!!\n";
                }
            }
        }


    }


    return 0;
}
