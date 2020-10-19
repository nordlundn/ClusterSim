#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "Queues.cpp"
#include <vector>
#include <math.h>

namespace py = pybind11;

const int numMachines = 10000;

class Cloud{
    private:
        int cloud[numMachines];
        int numBlocks = 128;

        ReadyQueue * readyQueue = new ReadyQueue();
        ProcessQueue * processQueue = new ProcessQueue();

    public:
        Cloud();
        ~Cloud();
        void printarray();
        void add(int, int);
        void schedule(double frac);
        int tick();
        int getMachineIndex(Task *);
        int getNumActive();
        double getUtilization();
        py::array getCloud();
        py::array getWaitTimes();
        int getNumFinished();
        int getMemFinished();
};

Cloud::Cloud(){
    for(int i = 0; i < numMachines; i++){
        cloud[i] = numBlocks;
    }
}

Cloud::~Cloud(){
    delete readyQueue;
    delete processQueue;
}

void Cloud::add(int mem, int dur){
    // std::cout << "Adding new job to ready queue \n";
    readyQueue->add(mem, dur);
}

void Cloud::printarray()
{
    readyQueue->printQueue();
    processQueue->printQueue();
    std::cout << "\n";
}
void Cloud::schedule(double frac){
    std::list<Task *> * queue = readyQueue->getWaiting();
    std::list<Task*>::iterator i = queue->begin();

    int numTasks = (int)round(frac*queue->size());
    // std::cout << queue->size() << "," << numTasks << "\n";

    Task * t;
    int machine;
    int count = 0;
    while (i != queue->end() && count < numTasks){
        t = *i;
        machine = getMachineIndex(t);
        if (machine >= 0){
            // std::cout << "This job can be scheduled \n";
            processQueue->add(t);
            i = queue->erase(i);
        }
        else{
            ++i;
        }
        count++;
    }
    // std::cout << "Remaining in queue: " << queue->size() << "\n";
}


int Cloud::tick(){
    processQueue->tick(cloud);
    return processQueue->getNumFinished();
}

int Cloud::getNumActive(){
    int active = 0;
    for (int i = 0; i < numMachines; i++){
        if (cloud[i] < numBlocks){
            active += 1;
        }
    }
    return active;
}

double Cloud::getUtilization(){
    double utilization = 0.0;
    double active = 0;
    for (int i = 0; i < numMachines; i++){
        if (cloud[i] < numBlocks){
            utilization += numBlocks - cloud[i];
            active += numBlocks;
        }
    }
    if (active == 0){
        return 0;
    }
    return utilization/active;
}

int Cloud::getMachineIndex(Task * t){
    int mem;
    for (int i = 0; i < numMachines; i++){
        mem = t->getRequest();
        if (cloud[i] >= mem){
            t->schedule(i);
            cloud[i] -= mem;
            return i;
        }
    }
    return -1;
}

py::array Cloud::getCloud(){
    std::vector<int> array(numMachines);
    for (int i = 0; i < numMachines; i++){
        array[i] = cloud[i];
    }
    return py::array(array.size(), array.data());

}

py::array Cloud::getWaitTimes(){
    return readyQueue->getWaitTimes();
}

int Cloud::getNumFinished(){
    return processQueue->getTotalFinished();
}

int Cloud::getMemFinished(){
    return processQueue->getMemFinished();
}

using namespace pybind11::literals;

PYBIND11_MODULE(EALA, m) {
    m.doc() = "pybind11 example plugin";
    py::class_<Cloud>(m, "Cloud")
        .def(py::init<>())
        .def("add", &Cloud::add)
        .def("tick", &Cloud::tick)
        .def("schedule", &Cloud::schedule)
        .def("getNumActive", &Cloud::getNumActive)
        .def("getUtilization", &Cloud::getUtilization)
        .def("getCloud", &Cloud::getCloud)
        .def("getWaitTimes", &Cloud::getWaitTimes)
        .def("getNumFinished", &Cloud::getNumFinished)
        .def("getMemFinished", &Cloud::getMemFinished)
        .def("printarray", &Cloud::printarray);
}