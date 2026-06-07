#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    // Inicializa el motor de la interfaz gráfica de Qt
    QApplication app(argc, argv);

    // Instancia y muestra nuestra ventana principal
    MainWindow window;
    window.show();

    // Inicia el bucle infinito para que la ventana no se cierre
    return app.exec();
}