#pragma once
#include <vector>
#include <memory>
#include "core/Process.h"
#include "schedulers/IScheduler.h"

class Simulator {
private:
    int currentTick;
    
    // Puntero al algoritmo actual (Polimorfismo)
    IScheduler* scheduler;

    // La "CPU" es simplemente un puntero al proceso en ejecución
    std::shared_ptr<Process> runningProcess;

    // Estructuras de datos (Colas)
    std::vector<std::shared_ptr<Process>> newProcesses;
    std::vector<std::shared_ptr<Process>> readyQueue;
    std::vector<std::shared_ptr<Process>> blockedQueue;
    std::vector<std::shared_ptr<Process>> terminatedQueue;

    // Estadísticas globales
    int totalIdleTicks;

public:
    Simulator();

    void setScheduler(IScheduler* newScheduler);
    void addProcess(std::shared_ptr<Process> process);
    
    // El motor principal del simulador
    void advanceTick();

    // Getters para UI y Estadísticas
    int getCurrentTick() const;
    const std::vector<std::shared_ptr<Process>>& getReadyQueue() const;
    const std::vector<std::shared_ptr<Process>>& getBlockedQueue() const;
    std::shared_ptr<Process> getRunningProcess() const;
};