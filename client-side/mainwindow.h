#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "progressdialog.h"
#include "qextserialport.h"
#include "qextserialenumerator.h"
#include <zlib.h>
#include "aboutdialog.h"

#include <QtGui>

struct BoardType
{
    QString id;
    QString name;
};
Q_DECLARE_METATYPE(BoardType)

struct RCInput
{
    QString id;
    QString name;
};
Q_DECLARE_METATYPE(RCInput)

struct RCInputMapping
{
    QString id;
    QString name;
};
Q_DECLARE_METATYPE(RCInputMapping)

struct Platform
{
    QString id;
    QString name;
    QString version;
    QString image;
};
Q_DECLARE_METATYPE(Platform)

struct GpsType
{
    QString id;
    QString name;
};
Q_DECLARE_METATYPE(GpsType)

struct GpsBaudrate
{
    QString id;
    QString name;
};
Q_DECLARE_METATYPE(GpsBaudrate)

struct Version
{
    QString id;
    QString platform;
    QString number;
};
typedef QList<Version> VersionsList;
Q_DECLARE_METATYPE(Version)

struct GlobalSettings
{
    QString hexurl;
};
Q_DECLARE_METATYPE(GlobalSettings)

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
    void downloadFinishedConfigs(DownloadsList downloads);
    void platformChanged(int index);
    void startFlash();
    void downloadFinishedFirmware(DownloadsList downloads);
    void firmwareRequestDone(DownloadsList downloads);
    void canceledDownloadFirmware();
    void avrdudeReadStandardOutput();
    void avrdudeReadStandardError();
    void avrdudeFinished(int exitCode);
    void canceledFirmwareUpload();
    void retryFirmwareDownload();
    void about();

private:
    Ui::MainWindow *ui;
    ProgressDialog *m_progressDialog;
    VersionsList m_versionList;
    QSettings m_settings;
    GlobalSettings m_globalsettings;
    QProcess *m_process;
    QString m_avrdudeOutput;
    DownloadsList m_currentFirmwareDownloads;
    QTimer *m_retrydownloads;
    AboutDialog *m_aboutDlg;

    void flashFirmware(QString filename, QString md5Filename);
    void parseAvrdudeOutput();
    QByteArray gzipDecompress(QByteArray compressData);
};

#endif // MAINWINDOW_H
