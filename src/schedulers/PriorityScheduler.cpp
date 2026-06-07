#include "schedulers/PriorityScheduler.h"
#include <algorithm>

PriorityScheduler::PriorityScheduler(bool isPreemptive) : preemptive(isPreemptive) {}

std::shared_ptr<Process> PriorityScheduler::selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) {
    if (readyQueue.empty()) return nullptr;

    // Buscar el proceso con el número de prioridad MÁS BAJO (Mayor urgencia)
    auto highestPriorityJob = std::min_element(readyQueue.begin(), readyQueue.end(),
        [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
            return a->getPriority() < b->getPriority();
        });

    return *highestPriorityJob;
}

bool PriorityScheduler::shouldPreempt(std::shared_ptr<Process> currentProcess, const std::vector<std::shared_ptr<Process>>& readyQueue) {
    // Si se configuró como NO expulsivo, o la cola está vacía, retornar falso inmediatamente
    if (!preemptive || readyQueue.empty()) return false;

    // Buscar el de mayor prioridad en la cola de listos
    auto highestPriorityInQueue = std::min_element(readyQueue.begin(), readyQueue.end(),
        [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
            return a->getPriority() < b->getPriority();
        });

    // Expulsar solo si el proceso en cola tiene MAYOR urgencia (menor número) que el proceso actual
    return (*highestPriorityInQueue)->getPriority() < currentProcess->getPriority();
}