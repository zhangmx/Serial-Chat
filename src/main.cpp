#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("Serial Chat");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("SerialChat");

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
