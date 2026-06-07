#include <iostream>
#include <memory>
#include "core/Simulator.h"
#include "schedulers/SrtfScheduler.h"
#include "schedulers/PriorityScheduler.h"

void runSimulation(Simulator& sim, int totalTicks) {
    std::shared_ptr<Process> prevRunning = nullptr;
    
    for (int i = 0; i < totalTicks; ++i) {
        sim.advanceTick();
        auto running = sim.getRunningProcess();
        
        // Solo imprimimos cuando hay un cambio en la CPU para ver las expulsiones claramente
        if (running != prevRunning) {
            std::cout << "[Tick " << sim.getCurrentTick() << "] Cambio: ";
            if (running) {
                std::cout << "Entra P" << running->getPid() 
                          << " | Restante: " << running->getRemainingCpuTime() << "t"
                          << " | Prioridad: " << running->getPriority() << "\n";
            } else {
                std::cout << "CPU Ociosa.\n";
            }
            prevRunning = running;
        }
    }

    // Imprimir estadísticas finales de la simulación actual
    auto terminados = sim.getTerminatedQueue();
    std::cout << "-> Procesos completados: " << terminados.size() << "\n";
    for (const auto& p : terminados) {
        std::cout << "   P" << p->getPid() << " espero " << p->getWaitTime() << "t en la cola.\n";
    }
}

int main() {
    // =========================================================
    // PRUEBA 1: SRTF (Primero el menor tiempo restante)
    // =========================================================
    std::cout << "=== PRUEBA 1: SRTF (Expulsivo) ===\n";
    Simulator simSRTF;
    SrtfScheduler srtf;
    simSRTF.setScheduler(&srtf);

    // P1 es largo, pero P2 llega un poco despues y es cortisimo
    simSRTF.addProcess(std::make_shared<Process>(1, 0, 15, 0, 1)); // P1: llega en 0, rafaga 15
    simSRTF.addProcess(std::make_shared<Process>(2, 3, 2, 0, 1));  // P2: llega en 3, rafaga 2
    
    runSimulation(simSRTF, 20);

    std::cout << "\n------------------------------------------------\n\n";

    // =========================================================
    // PRUEBA 2: PRIORIDAD EXPULSIVA
    // =========================================================
    std::cout << "=== PRUEBA 2: PRIORIDADES (Expulsivo) ===\n";
    Simulator simPrio;
    // Pasamos 'true' al constructor para activar la expulsion
    PriorityScheduler prio(true); 
    simPrio.setScheduler(&prio);

    // P1 llega primero pero es de prioridad 5 (Baja urgencia).
    // P2 llega despues con prioridad 1 (Alta urgencia).
    simPrio.addProcess(std::make_shared<Process>(1, 0, 10, 0, 5)); // P1: llega en 0, prioridad 5
    simPrio.addProcess(std::make_shared<Process>(2, 4, 5, 0, 1));  // P2: llega en 4, prioridad 1

    runSimulation(simPrio, 20);

    return 0;
}