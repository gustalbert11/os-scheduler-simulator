cmake_minimum_required(VERSION 3.16)
project(ProcessSimulator VERSION 1.0 LANGUAGES CXX)

# Requerir C++17 (Estándar moderno, útil para punteros inteligentes)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------
# Configuración específica de Qt
# ---------------------------------------------------------
# CMAKE_AUTOMOC: Maneja el Meta-Object Compiler de Qt (Signals/Slots)
set(CMAKE_AUTOMOC ON)
# CMAKE_AUTOUIC: Procesa archivos .ui de Qt Designer si decides usarlos
set(CMAKE_AUTOUIC ON)
# CMAKE_AUTORCC: Compila recursos como imágenes o íconos (.qrc)
set(CMAKE_AUTORCC ON)

# Buscar Qt (Soporta Qt5 o Qt6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

# ---------------------------------------------------------
# Archivos Fuente
# ---------------------------------------------------------
# Núcleo del simulador
set(CORE_SOURCES
    src/core/Process.cpp
    src/core/Simulator.cpp
)

# Algoritmos de planificación (Puedes ir descomentando a medida que los crees)
set(SCHEDULER_SOURCES
    src/schedulers/FcfsScheduler.cpp
    src/schedulers/RoundRobinScheduler.cpp
    src/schedulers/SjfScheduler.cpp
    src/schedulers/SrtfScheduler.cpp
    src/schedulers/RandomScheduler.cpp
    src/schedulers/PriorityScheduler.cpp
)

# Interfaz Gráfica (Ventanas y controles)
set(UI_SOURCES
    src/ui/MainWindow.cpp
    include/ui/MainWindow.h
)

# Archivo principal
set(MAIN_SOURCE src/main.cpp)

# ---------------------------------------------------------
# Construcción del Ejecutable
# ---------------------------------------------------------
add_executable(${PROJECT_NAME}
    ${CORE_SOURCES}
    ${SCHEDULER_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCE}
)

# ---------------------------------------------------------
# Directorios de Inclusión (Para que #include "core/Process.h" funcione)
# ---------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ---------------------------------------------------------
# Enlazado de Librerías (Vincular tu código con Qt)
# ---------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)