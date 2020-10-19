// #ifndef TASK
// #define TASK
// #include "Task.cpp"
// #include <list>
// #endif

#include "Task.cpp"
#include <list>

namespace py = pybind11;

class ReadyQueue{
    private:
        std::list<Task*> queue;
    public:
        ReadyQueue();
        ~ReadyQueue();
        void add(int, int);
        std::list<Task*> * getWaiting();
        void printQueue();
        py::array getWaitTimes();
};

ReadyQueue::ReadyQueue(){
}

ReadyQueue::~ReadyQueue(){
    std::list<Task*>::iterator i;
    Task * t;
    while (i != queue.end()){
        t = *i;
        delete t;
        i = queue.erase(i);
    }
}

void ReadyQueue::add(int mem, int dur){
    Task * task = new Task(mem, dur);
    queue.push_back(task);
}

std::list<Task*> * ReadyQueue::getWaiting(){
    std::list<Task*>::iterator i = queue.begin();
    bool overdue;
    Task * t;
    while (i != queue.end()){
        (*i)->tickWaiting();
        overdue = (*i)->isOverdue();
        // std::cout << overdue;
        // std::cout << (*i)->getTimeWaiting();
        if (overdue){
            t = *i;
            delete t;
            i = queue.erase(i);
        }
        else{
            ++i;
        }
    }
    return &queue;
}


void ReadyQueue::printQueue(){
    std::list<Task*>::iterator i;
    for(std::list<Task*>::iterator i = queue.begin();i != queue.end();i++)
    {
        (*i)->printTask();
    } 
}

py::array ReadyQueue::getWaitTimes(){
    std::vector<int> array(queue.size());
    std::list<Task*>::iterator i;
    int j = 0;
    for(std::list<Task*>::iterator i = queue.begin(); i != queue.end(); i++)
    {
        array[j] = (*i)->getTimeWaiting();
        j++;
    } 

    return py::array(array.size(), array.data());

}


class ProcessQueue{
    private:
        std::list<Task*> queue;
        int numFinished = 0;
        int totalFinished = 0;
        int memFinished = 0;
    public:
        ProcessQueue();
        ~ProcessQueue();
        void add(Task *);
        void tick(int[]);
        void printQueue();
        int getNumFinished();
        int getTotalFinished();
        int getMemFinished();
};

ProcessQueue::ProcessQueue(){}

ProcessQueue::~ProcessQueue(){
    std::list<Task*>::iterator i;
    Task * t;
    while (i != queue.end()){
        t = *i;
        delete t;
        i = queue.erase(i);
    }
}

void ProcessQueue::add(Task * t){
    queue.push_back(t);
}

void ProcessQueue::tick(int cloud[]){
    std::list<Task*>::iterator i = queue.begin();
    Task * t;
    bool finished;
    int location;
    int mem;
    while (i != queue.end()){
        (*i)->tickRunning();
        finished = (*i)->isFinished();
        if (finished) {
            t = *i;
            location = t->getLocation();
            mem = t->getRequest();
            cloud[location] += mem;
            memFinished += mem;
            totalFinished += 1;
            delete t;
            i = queue.erase(i);
            numFinished += 1;
            // std::cout << "This job has finished\n";
        }
        else{
            ++i;
        }
    }
}

void ProcessQueue::printQueue(){
    std::list<Task*>::iterator i;
    for(std::list<Task*>::iterator i = queue.begin();i != queue.end();i++)
    {
        (*i)->printTask();
    } 
}

int ProcessQueue::getNumFinished(){
    int toReturn = numFinished;
    numFinished = 0;
    return toReturn;
}

int ProcessQueue::getTotalFinished(){
    return totalFinished;
}

int ProcessQueue::getMemFinished(){
    return memFinished;
}
