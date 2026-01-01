#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application info
    app.setApplicationName("Serial Chat");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("SerialChat");
    app.setOrganizationDomain("serialchat.app");
    
    // Set application style
    app.setStyle("Fusion");
    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
