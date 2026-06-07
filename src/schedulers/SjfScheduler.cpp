#include "schedulers/SjfScheduler.h"
#include <algorithm>

std::shared_ptr<Process> SjfScheduler::selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) {
    if (readyQueue.empty()) {
        return nullptr;
    }

    // Encontrar el proceso con la menor ráfaga de CPU original
    auto shortestJob = std::min_element(readyQueue.begin(), readyQueue.end(),
        [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
            return a->getCpuBurstOriginal() < b->getCpuBurstOriginal();
        });

    return *shortestJob;
}