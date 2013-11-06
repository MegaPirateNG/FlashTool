#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

#include <QtGui>

struct Download
{
    QString uri;
    QString tmpFile;
    bool success;

    Download(QString uri)
    {
        this->uri = uri;
    }
};
typedef QList<Download> DownloadsList;

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

signals:
     void downloadsFinished(DownloadsList downloads);

public:
    explicit ProgressDialog(QWidget *parent = 0);
    void prepare(QString status, bool allowCancel);
    void startDownloads(DownloadsList downloads);
    void startDownloads(Download download);
    ~ProgressDialog();

private slots:
    void networkReplyFinished(QNetworkReply*);
    void networkReplyDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void networkReplyTimedOut();

private:
    Ui::ProgressDialog *ui;
    QTimer *m_downloadRequestTimeout;
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_networkRequest;
    int m_downloadsIndex;
    DownloadsList m_downloads;

    void doUrlDownload(QString url);
};

#endif // PROGRESSDIALOG_H
