#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "progressdialog.h"
#include "qextserialport.h"
#include "qextserialenumerator.h"

#include <QtGui>

struct BoardType
{
    QString id;
    QString name;
    QString patch;
};
Q_DECLARE_METATYPE(BoardType)

struct RCInput
{
    QString id;
    QString name;
    QString patch;
};
Q_DECLARE_METATYPE(RCInput)

struct Platform
{
    QString id;
    QString name;
    QString patch;
    QString version;
    QString image;
};
Q_DECLARE_METATYPE(Platform)

struct Version
{
    QString id;
    QString platform;
    QString number;
    QString source;
};
typedef QList<Version> VersionsList;
Q_DECLARE_METATYPE(Version)



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateSerialPorts();
    void updateConfigs();
    void parseConfigs(DownloadsList downloads);
    void platformChanged(int index);
    void startFlash();
    void prepareSourceCode(DownloadsList downloads);

private:
    Ui::MainWindow *ui;
    ProgressDialog *m_progressDialog;
    VersionsList m_versionList;
    QSettings *m_settings;
};

#endif // MAINWINDOW_H
