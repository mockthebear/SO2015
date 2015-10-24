#ifndef TASKHH
#define TASKHH

#include <string>
#include <sys/time.h>

class Task{
    public:
        Task(){
            delay = 0;
            amount=0;
            file="";
        };
        Task(int seconds,int amount,std::string file);
        static int taskId;
        bool isAlive(){
            return amount >= 1;
        };
        int getId(){
            return id;
        };
        bool RemoveThisJob(){
            amount=0;
        };
        std::string GetFileName(){
            return file;
        };
        int GetDelay(){
            return delay;
        };
        int GetTimesLeft(){
            return amount;
        };
        void Update();
        bool RunTrigger();
    
    private:
        int amount;
        int delay;
        std::string file;
        int id;
        struct timeval created,compare;
        bool canRun;
};

#endif // TASKHH
