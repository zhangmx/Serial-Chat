#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Serial Chat");
    app.setOrganizationName("SerialChat");
    app.setApplicationVersion("1.0.0");

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
