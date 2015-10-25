#include "../comunication.hpp"
#include "task.hpp"
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unistd.h>

// Configuracao local
#define QUANTUM 10

std::vector<int> Processos; //Pid de cada processo

pthread_mutex_t mutex;  //Essa mutex é a região critica do escalonador
                        //Evita que um processo seja adicionado enquanto deu o quantum do escalonador e ele esta escalonando
pthread_t escalonator;

bool Escalonator_Alive; //Se false = a thread do escalonador morre

void *escalonador_proc(void *param){
    std::cout << "[Server]Hey I am the thread [2] and I am the escalonator.\n";
    
    struct timeval Quantum, auxT;
    int LastOnQuantum = -1;
    int ProcessPointer = -1;
    gettimeofday(&Quantum, NULL);
    
    while (1){
        gettimeofday(&auxT, NULL);
        unsigned int mtime = (int) (1000 * (auxT.tv_sec - Quantum.tv_sec) + (auxT.tv_usec - Quantum.tv_usec) / 1000);
        
        if (mtime >= QUANTUM*1000){
            pthread_mutex_lock(&mutex);

                //Manipulando processos...
                if (Processos.size() > 0){
                    //Escalona
                    ProcessPointer++;
                    if (ProcessPointer >= Processos.size()){
                        ProcessPointer = 0;
                    }
                    std::cout << "[Info]Quantum {"<<ProcessPointer<<"}\n";
                    int status;
                    pid_t result = waitpid(Processos[ProcessPointer], &status, WNOHANG);
                    
                    if (result != 0){
                        //Processo terminou
                        int deadProcess = Processos[ProcessPointer];
                        std::vector<int>::iterator position = std::find(Processos.begin(), Processos.end(),deadProcess);
                        if (position != Processos.end()){
                            Processos.erase(position);
                            int status_child;
                            wait(&status);
                            std::cout << "[Info]Process " << deadProcess << " succefully closed with status "<< status_child << "\n";
                        }else{
                            std::cout << "[Error]Cannot remove " << Processos[ProcessPointer] <<"\n";
                        }
                    }else{
                        //Processo rodando.
                        if (LastOnQuantum != -1){
                            std::cout << "[SIGSTOP] > " << Processos[LastOnQuantum] <<"\n";
                            kill(Processos[LastOnQuantum], SIGSTOP);
                        }
                        LastOnQuantum = ProcessPointer;
                        std::cout << "[SIGCONT] > " << Processos[ProcessPointer] <<"\n";
                        kill(Processos[ProcessPointer], SIGCONT);
                    }
                }
            pthread_mutex_unlock(&mutex);
            gettimeofday(&Quantum, NULL);
        }
        if (!Escalonator_Alive){
            std::cout << "[Server][2]I am closing...\n";
            //Kill all process in queue.
            //Make sure there is no zombies
            //Kill eveything else...

            //Stop the while...
            break;
        }
        usleep(10000);
    }
    //Close
    pthread_exit(NULL);
}

int main(){
    MessageQueue SendMessage(QUEUE_SEND,0x1337, IPC_CREAT | 0x1FF); //CREATE and GET.
    MessageQueue RecvMessage(QUEUE_RECV,0x1338, IPC_CREAT | 0x1FF); //CREATE and GET.
    
    if (!SendMessage.isWorking() or !RecvMessage.isWorking()){
        std::cout << "Not working!\n";
        return 1;
    }
    
    Escalonator_Alive = true;
    //Inicia a thread
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_unlock(&mutex);
    pthread_create(&escalonator, NULL, &escalonador_proc, NULL);
    std::cout << "[Server]Hey I am the thread [1] and I am the server.\n";
    std::vector<Task> Task_list;
    char msg[127];
    
    while (1){
        if (RecvMessage.Receive(msg,false)){
            if (msg[0] == 1){
                //Comando
                if (msg[1] == 's'){
                    std::cout << "[Server][1]Got signal to close\n";
                    SendMessage.Send("Closing, as you asked...");
                    pthread_mutex_lock(&mutex);
                    // The thread will wait
                    Escalonator_Alive = false;
                    pthread_mutex_unlock(&mutex);
                    // Wait then close
                    pthread_join(escalonator, NULL);
                    break;
                }else if (msg[1] == 'e'){
					int delay = 0;
					int amount = 1;
					char fname[100];
					delay = (msg[2]-1)*3600 + (msg[3]-1)*60;
					amount = msg[4]-1;
					
                    // i=5 -> the filename starts in [5]
					int i;
					for (i=5;msg[i] != 0;i++){
						fname[i-5] = msg[i];
					}

					fname[i-5] = 0;
					FILE*fp = fopen(fname,"rb");
					
                    if (fp != NULL){
                        fclose(fp);
                        Task localJob = Task(delay,amount,fname);
                        Task_list.emplace_back(localJob);
                        std::stringstream output;
                        output << "Job id is ["<<localJob.getId()<<"] " << fname << " will run "<< amount << " times every "<< delay << " seconds.\n";
                        std::cout <<output.str();
                        SendMessage.Send(output.str());
					}else{
					    std::cout << "[Server]Error, file "<< fname << " not found.\n";
                        SendMessage.Send("[Error]File not found.\n");
					}
				}else if (msg[1] == 'r'){
                    bool removed = false;
                    
                    for(int i=0;i<Task_list.size();i++){
                        if (Task_list[i].isAlive()){
                            if (Task_list[i].getId() == (int)(msg[2]-1)){
                                Task_list[i].RemoveThisJob();
                                removed = true;
                            }
                        }
                    }
                    
                    if (removed){
                        SendMessage.Send("Job removed.\n");
                    }else{
                        SendMessage.Send("There is no job with such id.\n");
                    }
				}else if (msg[1] == 'l'){
                    SendMessage.Send("[ID]\t[file]\t[delay]\t[times]\t[incr]\n");
                    
                    for(int i=0;i<Task_list.size();i++){
                        if (Task_list[i].isAlive()){
                            std::stringstream Data;
                            Data << Task_list[i].getId() << "\t" << Task_list[i].GetFileName() << "\t"
                            << Task_list[i].GetDelay() << "\t" << Task_list[i].GetTimesLeft() << "\t" << "??:??\n";
                            SendMessage.Send(Data.str());
                        }
                    }
                    //Just to close the list program.
                    SendMessage.Send("end");
				}
            }
        }
        //Task manager!
        if (!Task_list.empty()){
            for(int i=0;i<Task_list.size();i++){
                if (Task_list[i].isAlive()){
                    Task_list[i].Update();
                    
                    if (Task_list[i].RunTrigger()){
                        std::string name = Task_list[i].GetFileName();
                        FILE*fp = fopen(name.c_str(),"rb");
                        
                        if (fp != NULL){
                            fclose(fp);
                            int pid = fork();
                            
                            if (pid == -1){
                                std::cout << "Problemas aconteceram!\n";
                                return 1;
                            }else if (pid == 0){
                                //Execução
                                char *params[] = {""};
                                if ( execv(name.c_str(),params) < 0 ){
                                    //Funcionou!
                                }
                            }
                            //Para o processo recem criado.
                            kill(pid, SIGSTOP);
                            pthread_mutex_lock(&mutex);

                            std::cout << "[Server]Created process " << pid <<"\n";
                            Processos.emplace_back(pid);

                            pthread_mutex_unlock(&mutex);
                        }else{
                            std::cout << "[Server]Error, file "<< name << " not found.\n";
                        }
                    }
                }
            }
        }
        usleep(1000000); //Wait one second;
    }

    SendMessage.Close();
    RecvMessage.Close();
    std::cout << "[Server]Messege queue closed, as everything else. Bye!\n";
    return 0;
}
