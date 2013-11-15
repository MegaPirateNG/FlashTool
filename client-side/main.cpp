#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("MegaPirateNG");
    a.setOrganizationDomain("megapirateng.com");
    a.setApplicationName("FlashTool");

    QTranslator translator;
    translator.load(QString("mpng_flashtool_%1").arg(QLocale::languageToString(QLocale::system().language()).toLower()), ":/translations");
    // Just to check translation on machine with other language
//    translator.load(QString("mpng_flashtool_russian"), ":/translations");
    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
