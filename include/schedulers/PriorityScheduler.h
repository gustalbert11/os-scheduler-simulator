#pragma once
#include "schedulers/IScheduler.h"

class PriorityScheduler : public IScheduler {
private:
    bool preemptive; // Define si el algoritmo puede expulsar a la CPU

public:
    // Al instanciarlo, el usuario/UI decide si es expulsivo o no (por defecto false)
    PriorityScheduler(bool isPreemptive = false);

    std::shared_ptr<Process> selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) override;
    
    // Sobrescribimos la expulsión condicionada al booleano
    bool shouldPreempt(std::shared_ptr<Process> currentProcess, const std::vector<std::shared_ptr<Process>>& readyQueue) override;
};