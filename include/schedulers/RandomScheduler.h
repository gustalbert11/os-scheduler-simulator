#pragma once
#include "schedulers/IScheduler.h"

class RandomScheduler : public IScheduler {
public:
    std::shared_ptr<Process> selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) override;
    // Es no expulsivo, así que no sobrescribimos shouldPreempt
};