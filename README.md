# Simulador de Planificación de Procesos OS 🖥️⚙️

Un simulador interactivo de escritorio construido con C++ moderno y Qt. Diseñado para visualizar, analizar y comparar el comportamiento de los algoritmos clásicos de planificación de CPU en Sistemas Operativos.

## ✨ Características Principales

* **Motor Algorítmico Robusto:** Simulación precisa del reloj del procesador (ticks), cambios de contexto y manejo de ráfagas de E/S.
* **Múltiples Algoritmos Soportados:**
  * No Expulsivos: FCFS, SJF, Selección Aleatoria, Prioridades.
  * Expulsivos: Round Robin (con Quantum configurable), SRTF, Prioridades.
* **Interfaz Gráfica Moderna:** UI limpia y responsiva construida en Qt con estilo *glassmorphism*.
* **Monitoreo en Tiempo Real:** Visualización dinámica de la Cola de Listos, CPU en ejecución y Cola de Bloqueados por E/S.
* **Métricas Académicas:** Cálculo automático del tiempo promedio de espera y tiempo de retorno (turnaround).

---

## 🛠️ Requisitos Previos

Para compilar y ejecutar este proyecto, tu sistema debe contar con las siguientes herramientas:

* **Compilador C++:** Soporte para C++14 o superior (GCC, Clang o MSVC).
* **CMake:** Versión 3.10 o superior.
* **Qt Framework:** Versión 5 o 6 instalada y configurada en las variables de entorno. 
  * *En distribuciones basadas en Debian/Ubuntu:* `sudo apt install qtbase5-dev`
* **Make:** Herramienta de compilación estándar.

---

## 🚀 Instalación y Compilación

Este proyecto utiliza CMake para gestionar su compilación de manera multiplataforma. Sigue estos pasos en tu terminal para compilar el simulador desde el código fuente:

1. **Clonar el repositorio:**
   ```bash
   git clone <URL_DE_TU_REPOSITORIO>
   cd os-scheduler-simulator
