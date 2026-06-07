#pragma once
#include <vector>
#include <memory>
#include "core/Process.h"

class IScheduler {
public:
    virtual ~IScheduler() = default;

    // Retorna el siguiente proceso a ejecutar. Si retorna nullptr, la CPU queda inactiva.
    virtual std::shared_ptr<Process> selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) = 0;

    // Para algoritmos expulsivos (como Round Robin o SRTF). 
    // Por defecto retorna false para algoritmos no expulsivos (FCFS, SJF).
    virtual bool shouldPreempt(std::shared_ptr<Process> currentProcess, const std::vector<std::shared_ptr<Process>>& readyQueue) {
        return false; 
    }
    
    // Si necesitas notificar al algoritmo que hubo un cambio de contexto (ej. reiniciar quantum)
    virtual void onContextSwitch() {} 
};