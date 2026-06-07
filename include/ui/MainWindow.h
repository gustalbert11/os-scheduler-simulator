#pragma once
#include <QMainWindow>
#include <QTimer>
#include <memory>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "schedulers/FcfsScheduler.h"
#include "schedulers/SjfScheduler.h"
#include "schedulers/SrtfScheduler.h"
#include "schedulers/RoundRobinScheduler.h"
#include "schedulers/PriorityScheduler.h"
#include "schedulers/RandomScheduler.h"

#include "core/Process.h" 

class Simulator;

class MainWindow : public QMainWindow {
    Q_OBJECT 

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    std::shared_ptr<Simulator> simulator;
    QTimer* simulationTimer;
    std::shared_ptr<IScheduler> currentScheduler;
    int nextPid = 1; // Contador para autogenerar el PID

    // --- Controles del Panel de Configuración ---
    QComboBox* algorithmSelector;
    QSpinBox* quantumSpinBox;
    QSpinBox* tickTimeSpinBox;
    QPushButton* btnStartPause;
    QPushButton* btnStep;
    QPushButton* btnRandomProcess;
    QLabel* infoLabel;

    // --- Controles del Panel de Colas (Visualización) ---
    QTableWidget* readyQueueTable;
    QTableWidget* blockedQueueTable;
    QTableWidget* terminatedQueueTable;
    QLabel* currentCpuLabel;

    // --- Controles del Panel de Estadísticas ---
    QLabel* lblTotalTicks;
    QLabel* lblAvgWaitTime;
    QLabel* lblAvgTurnaroundTime;

    void setupUI();
    void createConfigPanel(QVBoxLayout* mainLayout);
    void createQueuesPanel(QVBoxLayout* mainLayout);
    void createStatsPanel(QVBoxLayout* mainLayout);
    void applySelectedAlgorithm();
    void updateReadyQueueTable(std::shared_ptr<Process> newProcess);
    void updateUI();

private slots:
    
    void advanceSimulationStep();
    void onAlgorithmChanged(int index);
    void onGenerateRandomProcess(); 
    void onStartPauseClicked();
};