#pragma once

// Process life cycle states
enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
};

class Process {
private:
    // --- Identifiers and Initial Parameters ---
    int pid;
    int arrivalTime;
    int cpuBurstOriginal;
    int ioBurstOriginal;
    int priority;

    // --- Dynamic State Variables ---
    int remainingCpuTime;
    int remainingIoTime;
    ProcessState currentState;

    // --- Metrics for Statistics ---
    int waitTime;
    int blockTime;
    int completionTime; 

public:
    // Constructor
    Process(int id, int arrival, int cpu, int io, int prio);

    // --- Action Methods (Called by Simulator on each tick) ---
    void executeCpuTick(); 
    void executeIoTick();  
    void incrementWaitTime(); 

    // --- Getters ---
    int getPid() const;
    int getArrivalTime() const;
    int getCpuBurstOriginal() const;
    int getPriority() const;
    int getRemainingCpuTime() const;
    int getRemainingIoTime() const;
    ProcessState getState() const;
    
    // Metrics
    int getWaitTime() const;
    int getBlockTime() const;
    int getTurnaroundTime() const; // Turnaround Time = Completion - Arrival

    // --- Setters ---
    void setState(ProcessState newState);
    void setCompletionTime(int currentTick);
    
    // Helpers
    bool hasFinishedCpu() const;
    bool hasFinishedIo() const;
};