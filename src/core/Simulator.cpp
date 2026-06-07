#include "core/Simulator.h"
#include <algorithm> // Para std::remove_if

Simulator::Simulator() : currentTick(0), scheduler(nullptr), runningProcess(nullptr), totalIdleTicks(0) {}

void Simulator::setScheduler(IScheduler* newScheduler) {
    scheduler = newScheduler;
}

void Simulator::addProcess(std::shared_ptr<Process> process) {
    newProcesses.push_back(process);
}

void Simulator::advanceTick() {
    // 1. ADMISIÓN: Mover procesos de NUEVO a LISTO si su tiempo de llegada es el tick actual
    for (auto it = newProcesses.begin(); it != newProcesses.end(); ) {
        if ((*it)->getArrivalTime() <= currentTick) {
            (*it)->setState(ProcessState::READY);
            readyQueue.push_back(*it);
            it = newProcesses.erase(it);
        } else {
            ++it;
        }
    }

    // 2. ENTRADA/SALIDA (E/S): Actualizar procesos bloqueados
    for (auto it = blockedQueue.begin(); it != blockedQueue.end(); ) {
        (*it)->executeIoTick();
        
        // Si terminó su ráfaga de E/S, vuelve a la cola de listos
        if ((*it)->hasFinishedIo()) {
            (*it)->setState(ProcessState::READY);
            readyQueue.push_back(*it);
            it = blockedQueue.erase(it);
        } else {
            ++it;
        }
    }

    // 3. ACTUALIZAR TIEMPOS DE ESPERA: Procesos que están en la cola de listos
    for (auto& process : readyQueue) {
        process->incrementWaitTime();
    }

    // 4. EJECUCIÓN CPU: Actualizar el proceso actualmente en ejecución
    if (runningProcess != nullptr) {
        runningProcess->executeCpuTick();

        // Verificar si el proceso terminó su ráfaga de CPU
        if (runningProcess->hasFinishedCpu()) {
            if (!runningProcess->hasFinishedIo()) {
                // Si le falta E/S, va a la cola de bloqueados
                runningProcess->setState(ProcessState::BLOCKED);
                blockedQueue.push_back(runningProcess);
            } else {
                // Si no tiene E/S pendiente, finalizó completamente
                runningProcess->setState(ProcessState::TERMINATED);
                runningProcess->setCompletionTime(currentTick);
                terminatedQueue.push_back(runningProcess);
            }
            runningProcess = nullptr; // Liberar la CPU
        } 
        // Verificar expulsión (Preemption) si el algoritmo lo dicta (ej. Round Robin)
        else if (scheduler && scheduler->shouldPreempt(runningProcess, readyQueue)) {
            runningProcess->setState(ProcessState::READY);
            readyQueue.push_back(runningProcess);
            runningProcess = nullptr;
            scheduler->onContextSwitch();
        }
    } else {
        totalIdleTicks++; // La CPU estuvo ociosa este tick
    }

    // 5. PLANIFICACIÓN: Si la CPU está libre, pedirle al algoritmo el próximo proceso
    if (runningProcess == nullptr && scheduler != nullptr && !readyQueue.empty()) {
        runningProcess = scheduler->selectNextProcess(readyQueue);
        if (runningProcess != nullptr) {
            runningProcess->setState(ProcessState::RUNNING);
            
            // Eliminar el proceso seleccionado de la cola de listos
            auto it = std::find(readyQueue.begin(), readyQueue.end(), runningProcess);
            if (it != readyQueue.end()) {
                readyQueue.erase(it);
            }
        }
    }

    // 6. AVANZAR EL RELOJ
    currentTick++;
}

int Simulator::getCurrentTick() const { return currentTick; }
std::shared_ptr<Process> Simulator::getRunningProcess() const { return runningProcess; }
const std::vector<std::shared_ptr<Process>>& Simulator::getReadyQueue() const { return readyQueue; }
const std::vector<std::shared_ptr<Process>>& Simulator::getBlockedQueue() const { return blockedQueue; }