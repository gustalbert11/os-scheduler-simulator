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

    // === ESTILO VISUAL: GLASSMORPHISM Y DARK MODE ===
    // Fondo principal con gradiente
    centralWidget->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0f2027, stop:0.5 #203a43, stop:1 #2c5364);
            color: #ecf0f1;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        QGroupBox {
            background-color: rgba(255, 255, 255, 10);
            border: 1px solid rgba(255, 255, 255, 40);
            border-radius: 10px;
            margin-top: 2ex;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 5px;
            color: #3498db;
        }
        QPushButton {
            background-color: rgba(52, 152, 219, 40);
            border: 1px solid rgba(52, 152, 219, 80);
            border-radius: 6px;
            padding: 8px 15px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(52, 152, 219, 80);
        }
        QPushButton:disabled {
            background-color: rgba(127, 140, 141, 20);
            color: #7f8c8d;
        }
        QTableWidget {
            background-color: rgba(0, 0, 0, 40);
            alternate-background-color: rgba(255, 255, 255, 5);
            gridline-color: rgba(255, 255, 255, 20);
            border: 1px solid rgba(255, 255, 255, 30);
            border-radius: 5px;
        }
        QHeaderView::section {
            background-color: rgba(0, 0, 0, 60);
            padding: 4px;
            border: none;
            font-weight: bold;
        }
        QComboBox, QSpinBox {
            background-color: rgba(0, 0, 0, 50);
            border: 1px solid rgba(255, 255, 255, 50);
            border-radius: 4px;
            padding: 4px;
        }
    )");
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
    
    // Layout principal de este panel será VERTICAL
    QVBoxLayout* groupLayout = new QVBoxLayout(configGroup);

    // --- 1. FILA DE CONTROLES (Horizontal) ---
    QHBoxLayout* controlsLayout = new QHBoxLayout();

    controlsLayout->addWidget(new QLabel("Algoritmo:"));
    algorithmSelector = new QComboBox();
    algorithmSelector->addItems({"FCFS", "SJF", "SRTF", "Round Robin", "Prioridades (No Expulsivo)", "Prioridades (Expulsivo)", "Aleatorio"});
    connect(algorithmSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onAlgorithmChanged);
    controlsLayout->addWidget(algorithmSelector);

    controlsLayout->addWidget(new QLabel("Quantum:"));
    quantumSpinBox = new QSpinBox();
    quantumSpinBox->setRange(1, 100);
    quantumSpinBox->setEnabled(false);
    connect(quantumSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::applySelectedAlgorithm);
    controlsLayout->addWidget(quantumSpinBox);

    controlsLayout->addWidget(new QLabel("Velocidad Tick (ms):"));
    tickTimeSpinBox = new QSpinBox();
    tickTimeSpinBox->setRange(100, 5000);
    tickTimeSpinBox->setValue(1000);
    tickTimeSpinBox->setSingleStep(100);
    controlsLayout->addWidget(tickTimeSpinBox);

    btnRandomProcess = new QPushButton("Generar Proceso Aleatorio");
    btnStep = new QPushButton("Avanzar 1 Tick");
    btnStartPause = new QPushButton("Iniciar Simulacion");

    connect(btnRandomProcess, &QPushButton::clicked, this, &MainWindow::onGenerateRandomProcess);
    connect(btnStep, &QPushButton::clicked, this, &MainWindow::advanceSimulationStep);
    connect(btnStartPause, &QPushButton::clicked, this, &MainWindow::onStartPauseClicked);

    controlsLayout->addWidget(btnRandomProcess);
    controlsLayout->addWidget(btnStep);
    controlsLayout->addWidget(btnStartPause);

    // --- 2. FILA DE INFORMACIÓN TEÓRICA ---
    infoLabel = new QLabel("Selecciona un algoritmo para ver su descripción.");
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("background-color: rgba(0,0,0,40); padding: 10px; border-radius: 5px; border-left: 4px solid #f39c12; margin-top: 10px;");

    // --- ENSAMBLAR ---
    groupLayout->addLayout(controlsLayout); // Ponemos los botones arriba
    groupLayout->addWidget(infoLabel);      // Ponemos el texto abajo

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
    readyQueueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 2. CPU Actual (Centro)
    QGroupBox* cpuGroup = new QGroupBox("CPU en Ejecucion");
    QVBoxLayout* cpuLayout = new QVBoxLayout(cpuGroup);
    currentCpuLabel = new QLabel("CPU OCIOSA");
    currentCpuLabel->setAlignment(Qt::AlignCenter);
    cpuGroup->setMinimumWidth(200);
    // Agregaremos estilos (QSS) más tarde para que parezca un monitor
    cpuLayout->addWidget(currentCpuLabel);
    queuesLayout->addWidget(cpuGroup);

    // 3. Cola de Bloqueados
    QGroupBox* blockedGroup = new QGroupBox("Bloqueados por E/S");
    QVBoxLayout* blockedLayout = new QVBoxLayout(blockedGroup);
    blockedQueueTable = new QTableWidget(0, 2);
    blockedQueueTable->setHorizontalHeaderLabels({"PID", "E/S Restante"});
    blockedQueueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
    QString algo = algorithmSelector->currentText();
    QString baseInfo = "<b>Parámetros de Generación:</b> Ráfaga CPU (2 a 12 ticks), Ráfaga E/S (0 a 4 ticks), Prioridad (1 a 5, donde 1 es la urgencia más alta).<br><br>";
    QString algoInfo = "";

    if (algo == "FCFS") {
        quantumSpinBox->setEnabled(false);
        algoInfo = "<b>FCFS (First-Come, First-Served):</b> El primer proceso en llegar a la cola es el primero en usar la CPU. <i>No expulsivo.</i>";
    } else if (algo == "SJF") {
        quantumSpinBox->setEnabled(false);
        algoInfo = "<b>SJF (Shortest Job First):</b> Selecciona el proceso con la ráfaga de CPU original más corta. <i>No expulsivo.</i>";
    } else if (algo == "SRTF") {
        quantumSpinBox->setEnabled(false);
        algoInfo = "<b>SRTF (Shortest Remaining Time First):</b> Selecciona el proceso al que le quede menos tiempo de ejecución. <i>Expulsivo.</i>";
    } else if (algo == "Round Robin") {
        quantumSpinBox->setEnabled(true);
        algoInfo = "<b>Round Robin:</b> Cada proceso recibe una porción de tiempo equitativa (Quantum). Si no termina, regresa a la cola. <i>Expulsivo.</i>";
    } else if (algo == "Prioridades (No Expulsivo)") {
        quantumSpinBox->setEnabled(false);
        algoInfo = "<b>Prioridades (No Expulsivo):</b> Ejecuta el proceso con el número de prioridad más bajo (mayor urgencia). No interrumpe ejecuciones.";
    } else if (algo == "Prioridades (Expulsivo)") {
        quantumSpinBox->setEnabled(false);
        algoInfo = "<b>Prioridades (Expulsivo):</b> Si llega un proceso más urgente a la cola, el proceso actual es retirado de la CPU inmediatamente.";
    } else if (algo == "Aleatorio") {
        quantumSpinBox->setEnabled(false);
        algoInfo = "<b>Aleatorio:</b> Selecciona el próximo proceso al azar. Usado únicamente para fines académicos y comparativos. <i>No expulsivo.</i>";
    }

    infoLabel->setText(baseInfo + algoInfo);
    
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