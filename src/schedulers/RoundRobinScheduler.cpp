#include "schedulers/RoundRobinScheduler.h"

RoundRobinScheduler::RoundRobinScheduler(int q) : quantum(q), currentQuantumTicks(0) {}

std::shared_ptr<Process> RoundRobinScheduler::selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) {
    if (readyQueue.empty()) {
        return nullptr;
    }
    // Sigue siendo una cola: tomamos el primero
    return readyQueue.front(); 
}

bool RoundRobinScheduler::shouldPreempt(std::shared_ptr<Process> currentProcess, const std::vector<std::shared_ptr<Process>>& readyQueue) {
        currentQuantumTicks++;
        return currentQuantumTicks >= quantum;
    }
void RoundRobinScheduler::onContextSwitch() {
    // Al sacar un proceso o meter uno nuevo, el contador de quantum vuelve a cero
    currentQuantumTicks = 0;
}