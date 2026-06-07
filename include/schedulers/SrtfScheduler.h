#pragma once
#include "schedulers/IScheduler.h"

class SrtfScheduler : public IScheduler {
public:
    std::shared_ptr<Process> selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) override;
    
    // Sobrescribimos para habilitar la expulsión por menor tiempo
    bool shouldPreempt(std::shared_ptr<Process> currentProcess, const std::vector<std::shared_ptr<Process>>& readyQueue) override;
};