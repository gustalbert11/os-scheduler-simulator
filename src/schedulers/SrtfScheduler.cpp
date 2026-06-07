#include "schedulers/SrtfScheduler.h"
#include <algorithm>

std::shared_ptr<Process> SrtfScheduler::selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) {
    if (readyQueue.empty()) return nullptr;

    // A diferencia de SJF (que usa ráfaga original), SRTF usa el tiempo RESTANTE
    auto shortestJob = std::min_element(readyQueue.begin(), readyQueue.end(),
        [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
            return a->getRemainingCpuTime() < b->getRemainingCpuTime();
        });

    return *shortestJob;
}

bool SrtfScheduler::shouldPreempt(std::shared_ptr<Process> currentProcess, const std::vector<std::shared_ptr<Process>>& readyQueue) {
    if (readyQueue.empty()) return false;

    // Buscamos el proceso más corto en la cola de listos
    auto shortestInQueue = std::min_element(readyQueue.begin(), readyQueue.end(),
        [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
            return a->getRemainingCpuTime() < b->getRemainingCpuTime();
        });

    // Si el proceso más corto en la cola tiene MENOS tiempo que el proceso actual en la CPU, lo expulsamos
    return (*shortestInQueue)->getRemainingCpuTime() < currentProcess->getRemainingCpuTime();
}