#pragma once
#include "schedulers/IScheduler.h"

class RoundRobinScheduler : public IScheduler {
private:
    int quantum;
    int currentQuantumTicks;

public:
    RoundRobinScheduler(int q);

    std::shared_ptr<Process> selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) override;
    
    // Aquí sí sobrescribimos la expulsión
    bool shouldPreempt(std::shared_ptr<Process> currentProcess, const std::vector<std::shared_ptr<Process>>& readyQueue) override;
    // Reseteamos el contador cuando cambia el proceso
    void onContextSwitch() override;
};