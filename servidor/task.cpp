#include "task.hpp"
#include <iostream>
int Task::taskId = 0;
Task::Task(int seconds_,int amount_,std::string file_)
{
    delay     =   seconds_;
    amount      =   amount_;
    file        =   file_;
    id = taskId++;
    gettimeofday(&created, NULL);
    canRun = false;
}

void Task::Update()
{
    gettimeofday(&compare, NULL);

    unsigned int mtime = (int) (1000 * (compare.tv_sec - created.tv_sec) + (compare.tv_usec - created.tv_usec) / 1000);
    if (mtime >= delay*1000){
        std::cout << "[Running] " << file << " - " << amount << ": " << delay << "\n";
        canRun = true;
        amount--;
        gettimeofday(&created, NULL);
    }
}

bool Task::RunTrigger()
{
    if (canRun){
        canRun = false;
        return true;
    }
    return false;
}
