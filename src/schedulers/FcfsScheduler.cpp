#include "schedulers/FcfsScheduler.h"

std::shared_ptr<Process> FcfsScheduler::selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) {
    if (readyQueue.empty()) {
        return nullptr; // No hay procesos para ejecutar
    }
    
    // FCFS: Retorna el primer proceso que entró a la cola de listos
    return readyQueue.front(); 
}