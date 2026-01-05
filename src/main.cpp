#include <QApplication>
#include <QIcon>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application info
    app.setApplicationName("Serial Chat");
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName("SerialChat");
    app.setOrganizationDomain("serialchat.app");
    
    // Set application icon
    app.setWindowIcon(QIcon(":/icons/app.svg"));
    
    // Set application style
    app.setStyle("Fusion");
    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
