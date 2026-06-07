#include "ui/MainWindow.h"
#include "core/Simulator.h"
#include <QHeaderView>
#include <QGroupBox>
#include <QTableWidgetItem>
#include <QString>
#include <random>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    simulator = std::make_shared<Simulator>();
    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &MainWindow::advanceSimulationStep);

    setWindowTitle("OS Process Scheduling Simulator");
    resize(1200, 800); 

    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Layout principal vertical
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20); // Espacio entre paneles
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Llamamos a los métodos que construyen cada sección
    createConfigPanel(mainLayout);
    createQueuesPanel(mainLayout);
    createStatsPanel(mainLayout);
    applySelectedAlgorithm();
}

void MainWindow::createConfigPanel(QVBoxLayout* mainLayout) {
    QGroupBox* configGroup = new QGroupBox("Configuracion de Simulacion");
    QHBoxLayout* configLayout = new QHBoxLayout(configGroup);

    // Selector de Algoritmo
    configLayout->addWidget(new QLabel("Algoritmo:"));
    algorithmSelector = new QComboBox();
    algorithmSelector->addItems({"FCFS", "SJF", "SRTF", "Round Robin", "Prioridades (No Expulsivo)", "Prioridades (Expulsivo)", "Aleatorio"});
    connect(algorithmSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onAlgorithmChanged);
    configLayout->addWidget(algorithmSelector);

    // Quantum (Solo habilitado para Round Robin)
    configLayout->addWidget(new QLabel("Quantum:"));
    quantumSpinBox = new QSpinBox();
    quantumSpinBox->setRange(1, 100);
    quantumSpinBox->setEnabled(false); // Deshabilitado por defecto
    connect(quantumSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::applySelectedAlgorithm);
    configLayout->addWidget(quantumSpinBox);

    configLayout->addWidget(new QLabel("Velocidad Tick (ms):"));
    tickTimeSpinBox = new QSpinBox();
    tickTimeSpinBox->setRange(100, 5000); // Entre 0.1s y 5s
    tickTimeSpinBox->setValue(1000); // 1 segundo por defecto
    tickTimeSpinBox->setSingleStep(100);
    configLayout->addWidget(tickTimeSpinBox);
    
    // Botones de acción
    btnRandomProcess = new QPushButton("Generar Proceso Aleatorio");
    connect(btnRandomProcess, &QPushButton::clicked, this, &MainWindow::onGenerateRandomProcess);
    btnStep = new QPushButton("Avanzar 1 Tick");
    connect(btnStep, &QPushButton::clicked, this, &MainWindow::advanceSimulationStep);
    btnStartPause = new QPushButton("Iniciar Simulacion");
    connect(btnStartPause, &QPushButton::clicked, this, &MainWindow::onStartPauseClicked);
    
    configLayout->addWidget(btnRandomProcess);
    configLayout->addWidget(btnStep);
    configLayout->addWidget(btnStartPause);

    mainLayout->addWidget(configGroup);
}

void MainWindow::createQueuesPanel(QVBoxLayout* mainLayout) {
    // Un layout horizontal para poner las colas lado a lado
    QHBoxLayout* queuesLayout = new QHBoxLayout();

    // 1. Cola de Listos
    QGroupBox* readyGroup = new QGroupBox("Cola de Listos (Ready)");
    QVBoxLayout* readyLayout = new QVBoxLayout(readyGroup);
    readyQueueTable = new QTableWidget(0, 3);
    readyQueueTable->setHorizontalHeaderLabels({"PID", "CPU Restante", "Prioridad"});
    readyLayout->addWidget(readyQueueTable);
    queuesLayout->addWidget(readyGroup);

    // 2. CPU Actual (Centro)
    QGroupBox* cpuGroup = new QGroupBox("CPU en Ejecucion");
    QVBoxLayout* cpuLayout = new QVBoxLayout(cpuGroup);
    currentCpuLabel = new QLabel("CPU OCIOSA");
    currentCpuLabel->setAlignment(Qt::AlignCenter);
    // Agregaremos estilos (QSS) más tarde para que parezca un monitor
    cpuLayout->addWidget(currentCpuLabel);
    queuesLayout->addWidget(cpuGroup);

    // 3. Cola de Bloqueados
    QGroupBox* blockedGroup = new QGroupBox("Bloqueados por E/S");
    QVBoxLayout* blockedLayout = new QVBoxLayout(blockedGroup);
    blockedQueueTable = new QTableWidget(0, 2);
    blockedQueueTable->setHorizontalHeaderLabels({"PID", "E/S Restante"});
    blockedLayout->addWidget(blockedQueueTable);
    queuesLayout->addWidget(blockedGroup);

    mainLayout->addLayout(queuesLayout);
}

void MainWindow::createStatsPanel(QVBoxLayout* mainLayout) {
    QGroupBox* statsGroup = new QGroupBox("Metricas de Rendimiento");
    QHBoxLayout* statsLayout = new QHBoxLayout(statsGroup);

    lblTotalTicks = new QLabel("Ticks Transcurridos: 0");
    lblAvgWaitTime = new QLabel("Tiempo Promedio Espera: 0t");
    lblAvgTurnaroundTime = new QLabel("Tiempo Promedio Retorno: 0t");

    statsLayout->addWidget(lblTotalTicks);
    statsLayout->addWidget(lblAvgWaitTime);
    statsLayout->addWidget(lblAvgTurnaroundTime);

    mainLayout->addWidget(statsGroup);
}

void MainWindow::onAlgorithmChanged(int index) {
    // Si el índice corresponde a Round Robin (posición 3 en nuestra lista), habilitamos el Quantum
    if (algorithmSelector->currentText() == "Round Robin") {
        quantumSpinBox->setEnabled(true);
    } else {
        quantumSpinBox->setEnabled(false);
    }
    // Aplicar el nuevo algoritmo al simulador
    applySelectedAlgorithm();
}

void MainWindow::advanceSimulationStep() {
    if (simulator) {
        // 1. El motor avanza el tiempo matemáticamente
        simulator->advanceTick();
        
        // 2. La interfaz se repinta para reflejar los cambios
        updateUI();
    }
}

void MainWindow::onGenerateRandomProcess() {
    // Generador de números aleatorios moderno
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Parámetros exigidos por el proyecto
    std::uniform_int_distribution<> burstDist(2, 12);  // Ráfaga CPU entre 2 y 12
    std::uniform_int_distribution<> ioDist(0, 4);      // Ráfaga E/S entre 0 y 4 (0 = sin E/S)
    std::uniform_int_distribution<> prioDist(1, 5);    // Prioridad del 1 al 5

    int cpuBurst = burstDist(gen);
    int ioBurst = ioDist(gen);
    int priority = prioDist(gen);
    
    // El proceso llega en el tick actual del simulador
    int arrivalTime = simulator->getCurrentTick();

    // Instanciar el proceso en nuestro backend
    auto newProcess = std::make_shared<Process>(nextPid++, arrivalTime, cpuBurst, ioBurst, priority);
    
    // Agregarlo al motor de simulación
    simulator->addProcess(newProcess);

    // Actualizar la interfaz gráfica
    updateReadyQueueTable(newProcess);
}

void MainWindow::updateReadyQueueTable(std::shared_ptr<Process> process) {
    // Insertar una nueva fila al final de la tabla
    int row = readyQueueTable->rowCount();
    readyQueueTable->insertRow(row);

    // Columna 0: PID
    readyQueueTable->setItem(row, 0, new QTableWidgetItem("P" + QString::number(process->getPid())));
    
    // Columna 1: Ráfaga CPU
    readyQueueTable->setItem(row, 1, new QTableWidgetItem(QString::number(process->getRemainingCpuTime()) + "t"));
    
    // Columna 2: Prioridad
    readyQueueTable->setItem(row, 2, new QTableWidgetItem(QString::number(process->getPriority())));

    // Centrar el texto en las celdas para que se vea profesional
    for (int col = 0; col < 3; ++col) {
        readyQueueTable->item(row, col)->setTextAlignment(Qt::AlignCenter);
    }
}

void MainWindow::updateUI() {
    // === 1. ACTUALIZAR PANEL DE ESTADÍSTICAS ===
    lblTotalTicks->setText("Ticks Transcurridos: " + QString::number(simulator->getCurrentTick()));
    
    // === 2. ACTUALIZAR LA CPU VISUAL ===
    auto runningProcess = simulator->getRunningProcess();
    if (runningProcess) {
        currentCpuLabel->setText("EJECUTANDO P" + QString::number(runningProcess->getPid()) + 
                                 "\nRestante: " + QString::number(runningProcess->getRemainingCpuTime()) + "t");
        // Le damos un color verde para indicar que está activa
        currentCpuLabel->setStyleSheet("QLabel { color: #2ecc71; font-size: 18px; font-weight: bold; }");
    } else {
        currentCpuLabel->setText("CPU OCIOSA");
        // Le damos un color rojo tenue cuando está inactiva
        currentCpuLabel->setStyleSheet("QLabel { color: #e74c3c; font-size: 18px; font-weight: bold; }");
    }

    // === 3. ACTUALIZAR LA COLA DE LISTOS ===
    // Primero, vaciamos la tabla por completo
    readyQueueTable->setRowCount(0);
    
    // Obtenemos la lista actualizada del motor
    auto readyQueue = simulator->getReadyQueue();
    
    // Llenamos la tabla fila por fila
    for (const auto& p : readyQueue) {
        int row = readyQueueTable->rowCount();
        readyQueueTable->insertRow(row);
        
        readyQueueTable->setItem(row, 0, new QTableWidgetItem("P" + QString::number(p->getPid())));
        readyQueueTable->setItem(row, 1, new QTableWidgetItem(QString::number(p->getRemainingCpuTime()) + "t"));
        readyQueueTable->setItem(row, 2, new QTableWidgetItem(QString::number(p->getPriority())));
        
        // Centrar el texto
        for (int col = 0; col < 3; ++col) {
            readyQueueTable->item(row, col)->setTextAlignment(Qt::AlignCenter);
        }
    }

    blockedQueueTable->setRowCount(0);
    auto blockedQueue = simulator->getBlockedQueue();
    
    for (const auto& p : blockedQueue) {
        int row = blockedQueueTable->rowCount();
        blockedQueueTable->insertRow(row);
        
        blockedQueueTable->setItem(row, 0, new QTableWidgetItem("P" + QString::number(p->getPid())));
        // Mostramos el tiempo de E/S que le falta cumplir
        blockedQueueTable->setItem(row, 1, new QTableWidgetItem(QString::number(p->getRemainingIoTime()) + "t"));
        
        for (int col = 0; col < 2; ++col) {
            blockedQueueTable->item(row, col)->setTextAlignment(Qt::AlignCenter);
        }
    }
    
    // === 4. ACTUALIZAR PROMEDIOS SI HAY PROCESOS TERMINADOS ===
    auto terminados = simulator->getTerminatedQueue();
    if (!terminados.empty()) {
        float sumaEspera = 0, sumaEjecucion = 0;
        for (const auto& p : terminados) {
            sumaEspera += p->getWaitTime();
            sumaEjecucion += p->getTurnaroundTime();
        }
        
        float promEspera = sumaEspera / terminados.size();
        float promRetorno = sumaEjecucion / terminados.size();
        
        // El formato 'f', 2 limita los decimales a dos dígitos
        lblAvgWaitTime->setText("Tiempo Promedio Espera: " + QString::number(promEspera, 'f', 2) + "t");
        lblAvgTurnaroundTime->setText("Tiempo Promedio Retorno: " + QString::number(promRetorno, 'f', 2) + "t");
    }
}

void MainWindow::applySelectedAlgorithm() {
    QString algo = algorithmSelector->currentText();
    
    if (algo == "FCFS") {
        currentScheduler = std::make_shared<FcfsScheduler>();
    } else if (algo == "SJF") {
        currentScheduler = std::make_shared<SjfScheduler>();
    } else if (algo == "SRTF") {
        currentScheduler = std::make_shared<SrtfScheduler>();
    } else if (algo == "Round Robin") {
        // Le pasamos el valor que tenga la cajita de Quantum
        currentScheduler = std::make_shared<RoundRobinScheduler>(quantumSpinBox->value());
    } else if (algo == "Prioridades (No Expulsivo)") {
        currentScheduler = std::make_shared<PriorityScheduler>(false);
    } else if (algo == "Prioridades (Expulsivo)") {
        currentScheduler = std::make_shared<PriorityScheduler>(true);
    } else if (algo == "Aleatorio") {
        currentScheduler = std::make_shared<RandomScheduler>();
    }

    // Le inyectamos el cerebro seleccionado al simulador
    if (simulator && currentScheduler) {
        simulator->setScheduler(currentScheduler.get());
    }
}

void MainWindow::onStartPauseClicked() {
    if (simulationTimer->isActive()) {
        // Si ya está corriendo, lo pausamos
        simulationTimer->stop();
        btnStartPause->setText("Iniciar Simulacion");
        btnStartPause->setStyleSheet(""); // Quitamos colores
        btnStep->setEnabled(true); // Habilitamos el avance manual
    } else {
        // Si está detenido, lo iniciamos con la velocidad seleccionada
        int speedMs = tickTimeSpinBox->value();
        simulationTimer->start(speedMs);
        
        btnStartPause->setText("Pausar Simulacion");
        btnStartPause->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold;"); // Botón rojo
        btnStep->setEnabled(false); // Deshabilitamos el avance manual mientras corre
    }
}