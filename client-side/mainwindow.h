#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "progressdialog.h"
#include <zlib.h>
#include "aboutdialog.h"
#include <QSerialPortInfo>
#include <QSerialPort>

#include <QtGui>
#include "F4BYFirmwareUploader.h"

struct BoardType
{
    BoardType() :
        showInputs(true),
        showGPS(true),
        useBootloader(false)
    {

    }

    QString id;
    QString name;
    bool showInputs;
    bool showGPS;
    bool useBootloader;
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
    QStringList boards;
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
    void boardChanged(int index);
    void startFlash();
    void downloadFinishedFirmware(DownloadsList downloads);
    void firmwareRequestDone(DownloadsList downloads);
    void canceledDownloadFirmware();
    void avrdudeReadStandardOutput();
    void avrdudeReadStandardError();
    void avrdudeFinished(int exitCode);
    void avrdudeError(QProcess::ProcessError error);
    void canceledFirmwareUpload();
    void retryFirmwareDownload();
    void px4firmwareDownloadProgress(qint64 all, qint64 cur);
    void about();

    void px4StatusUpdate(QString status);
    void px4devicePlugDetected();
    void px4requestDeviceReplug();
    void px4Terminated();
    void px4Finished();
    void px4Error(QString error);
    void px4Warning(QString warning);
    void px4firmwareUpdateCancelled();

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
    QString m_firmwareFileName;
    QString m_firmwareDirectoryName;
    AboutDialog *m_aboutDlg;
    F4BYFirmwareUploader* m_px4uploader;
    bool m_isF4BY;

    void flashFirmware(QString filename);
    void parseAvrdudeOutput();
    QByteArray gzipDecompress(QByteArray compressData);
};

#endif // MAINWINDOW_H
