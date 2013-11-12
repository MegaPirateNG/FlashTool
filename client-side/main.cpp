#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("MegaPirateNG");
    a.setOrganizationDomain("megapirateng.com");
    a.setApplicationName("FlashTool");
    MainWindow w;
    w.show();

    return a.exec();
}
