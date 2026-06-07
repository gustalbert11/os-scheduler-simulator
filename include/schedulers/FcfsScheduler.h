#pragma once
#include "schedulers/IScheduler.h"

class FcfsScheduler : public IScheduler {
public:
    // Solo necesitamos sobrescribir el método de selección
    std::shared_ptr<Process> selectNextProcess(std::vector<std::shared_ptr<Process>>& readyQueue) override;
    
    // No necesitamos sobrescribir shouldPreempt() porque FCFS no es expulsivo, 
    // y la clase base ya retorna false por defecto.
};