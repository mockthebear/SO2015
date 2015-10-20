#include "../comunication.hpp"
#include "task.hpp"
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <vector>
#include <sstream>
// Config local
#define QUANTUM 10
/*
    Isso pode ser encapsulado em uma classe. Mas por hora, não.
*/
std::vector<int> Processos;
pthread_mutex_t mutex;
pthread_t escalonator;
bool Escalonator_Alive;
/*

*/

void *escalonador_proc(void *param)
{
    //Aqui os processos serão escalonados de forma paralela!!!1!!!!11
    std::cout << "[Server]Hey i am the thread [2] and i am the escalonator.\n";
    struct timeval Quantum, auxT;
    int LastOnQuantum = -1;
    gettimeofday(&Quantum, NULL);
    while (1){
        gettimeofday(&auxT, NULL);
        unsigned int mtime = (int) (1000 * (auxT.tv_sec - Quantum.tv_sec) + (auxT.tv_usec - Quantum.tv_usec) / 1000);
        if (mtime >= QUANTUM*1000){
            pthread_mutex_lock(&mutex);
                //Manipulando processos...
                if (Processos.size() > 0){
                    //Escalona
                }
            pthread_mutex_unlock(&mutex);
            gettimeofday(&Quantum, NULL);
        }
        if (!Escalonator_Alive){
            std::cout << "[Server][2]i am closing...\n";
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
        std::cout << "Not working :c\n";
        return 1;
    }
    Escalonator_Alive = true;
    //Inicia a thread
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_unlock(&mutex);
    pthread_create(&escalonator, NULL, &escalonador_proc, NULL);
    std::cout << "[Server]Hey i am the thread [1] and i am the server.\n";
    std::vector<Task> Task_list;
    char msg[127];
    while (1){
        if (RecvMessage.Receive(msg,false)){
            if (msg[0] == 1){
                //Comando
                if (msg[1] == 's'){
                    std::cout << "[Server][1]Got signal to close\n";
                    SendMessage.Send("Closing as you asked");
                    pthread_mutex_lock(&mutex);
                    /*
                        Just to make sure that the thread will wait
                    */
                    Escalonator_Alive = false;
                    pthread_mutex_unlock(&mutex);
                    /*
                        Wait here, then close
                    */
                    pthread_join(escalonator, NULL);
                    break;
                }else if (msg[1] == 'e'){
					int delay = 0;
					int amount = 1;
					char fname[100];
					delay = (msg[2]-1)*60 + (msg[3]-1);
					amount = msg[4]-1;
					//i=5 because the filename starts in [5]
					int i;
					for (i=5;msg[i] != 0;i++){
						fname[i-5] = msg[i];
					}
					fname[i-5] = 0;
					Task localJob = Task(delay,amount,fname);
					Task_list.emplace_back(localJob);
					/*
                        Todo: ela quer que exiba assim:
                        Executando <programa> as horario 1, horario 2, horario 3...
					*/
					std::stringstream output;
					output << "Job id is ["<<localJob.getId()<<"] " << fname << " will run "<< amount<< " times every "<< delay<< " seconds.";
					std::cout <<output.str();
					SendMessage.Send(output.str());
				}else if (msg[1] == 'r'){
                    bool removed = false;
                    for(int i=0;i<Task_list.size();i++)
                    {
                        if (Task_list[i].isAlive())
                        {
                            if (Task_list[i].getId() == (int)(msg[2]-1))
                            {
                                Task_list[i].RemoveThisJob();
                                removed = true;
                            }
                        }
                    }
                    if (removed)
                    {
                        SendMessage.Send("Job removed.");
                    }
                    else
                    {
                        SendMessage.Send("There is no job with such id.");
                    }

				}else if (msg[1] == 'l'){
                    SendMessage.Send("[ID]\t[file]\t[delay]\t[times]\t[incr]\n");
                    for(int i=0;i<Task_list.size();i++)
                    {
                        if (Task_list[i].isAlive())
                        {
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
        if (!Task_list.empty())
        {
            for(int i=0;i<Task_list.size();i++)
            {
                if (Task_list[i].isAlive()){
                    Task_list[i].Update();
                    if (Task_list[i].RunTrigger()){
                        pthread_mutex_lock(&mutex);
                        /*
                            Add processo;
                            std::string name = Task_list[i].GetFileName();
                        */
                        pthread_mutex_unlock(&mutex);

                    }
                }
            }
        }
        //Wait one second;
        usleep(1000000);
    }

    SendMessage.Close();
    RecvMessage.Close();
    std::cout << "[Server]Messege queue closed, as everything else. Bye!\n";
    return 0;
}
