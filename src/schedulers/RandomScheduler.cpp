#include "schedulers/RandomScheduler.h"
#include <random>

std::shared_ptr<Process> RandomScheduler::selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) {
    if (readyQueue.empty()) return nullptr;

    // Generador de números aleatorios en C++ moderno
    std::random_device rd;  // Semilla de hardware
    std::mt19937 gen(rd()); // Motor Mersenne Twister
    std::uniform_int_distribution<> distrib(0, readyQueue.size() - 1); // Rango

    int randomIndex = distrib(gen);
    return readyQueue[randomIndex];
}