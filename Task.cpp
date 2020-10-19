#include <iostream>

int maxWaitTime = 500;

class Task{
    private:
        int memoryRequestSize;
        int timeRemaining;
        int scheduledLocation = -1;
        bool scheduled = false;
        bool finished = false;
        bool overdue = false;
        int timeWaiting = 0;

    public:
        Task(int, int);
        bool isScheduled();
        bool isOverdue();
        bool isFinished();
        void tickRunning();
        void tickWaiting();
        void schedule(int);
        int getRequest();
        int getLocation();    
        void printTask();
        int getTimeWaiting();
};

Task::Task(int mem, int dur){
    memoryRequestSize = mem;
    timeRemaining = dur;
}

bool Task::isScheduled(){
    return scheduled;
}

bool Task::isOverdue(){
    return overdue;
}
bool Task::isFinished(){
    return finished;
}

void Task::tickRunning(){
    timeRemaining--;
    if (timeRemaining == 0){
        finished = true;
    }
}

void Task::tickWaiting(){
    timeWaiting++;
    if (timeWaiting > maxWaitTime){
        overdue = true;
    }
}

void Task::schedule(int location){
    scheduledLocation = location;
    scheduled = true;
}

int Task::getRequest(){
    return memoryRequestSize;
}

int Task::getLocation(){
    return scheduledLocation;
}

void Task::printTask(){
    std::cout << memoryRequestSize << ", " << timeRemaining << ", " << scheduledLocation <<"\n";
}

int Task::getTimeWaiting(){
    return timeWaiting;
}