#include "core/Process.h"

// Constructor: Initializes original values and sets dynamic variables
Process::Process(int id, int arrival, int cpu, int io, int prio)
    : pid(id), 
      arrivalTime(arrival), 
      cpuBurstOriginal(cpu), 
      ioBurstOriginal(io), 
      priority(prio),
      remainingCpuTime(cpu), 
      remainingIoTime(io),
      currentState(ProcessState::NEW),
      waitTime(0), 
      blockTime(0), 
      completionTime(0) {
}

// Action Methods
void Process::executeCpuTick() {
    if (remainingCpuTime > 0) {
        remainingCpuTime--;
    }
}

void Process::executeIoTick() {
    if (remainingIoTime > 0) {
        remainingIoTime--;
        blockTime++; // Increments the metric for time spent in Blocked state
    }
}

void Process::incrementWaitTime() {
    waitTime++;
}

// Getters
int Process::getPid() const { return pid; }
int Process::getArrivalTime() const { return arrivalTime; }
int Process::getCpuBurstOriginal() const { return cpuBurstOriginal; }
int Process::getPriority() const { return priority; }
int Process::getRemainingCpuTime() const { return remainingCpuTime; }
int Process::getRemainingIoTime() const { return remainingIoTime; }
ProcessState Process::getState() const { return currentState; }

// Metrics
int Process::getWaitTime() const { return waitTime; }
int Process::getBlockTime() const { return blockTime; }

int Process::getTurnaroundTime() const {
    // Turnaround is calculated only when the process is completed
    if (completionTime == 0) return 0; 
    return completionTime - arrivalTime;
}

// Setters
void Process::setState(ProcessState newState) {
    currentState = newState;
}

void Process::setCompletionTime(int currentTick) {
    completionTime = currentTick;
}

// Helpers
bool Process::hasFinishedCpu() const {
    return remainingCpuTime <= 0;
}

bool Process::hasFinishedIo() const {
    return remainingIoTime <= 0;
}