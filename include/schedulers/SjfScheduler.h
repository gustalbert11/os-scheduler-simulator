#pragma once
#include "schedulers/IScheduler.h"

class SjfScheduler : public IScheduler {
public:
    std::shared_ptr<Process> selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) override;
    // No sobrescribimos shouldPreempt() porque SJF es no expulsivo
};