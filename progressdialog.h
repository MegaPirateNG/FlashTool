#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QProgressDialog>
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

class ProgressDialog : public QProgressDialog
{
    Q_OBJECT

signals:
     void downloadsFinished(DownloadsList downloads);

public:
    explicit ProgressDialog();
    void startDownloads(DownloadsList downloads);
    void startDownloads(Download download);

private slots:
    void networkReplyFinished(QNetworkReply*);
    void networkReplyDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void networkReplyTimedOut();
    void onCanceled();

private:
    QTimer *m_downloadRequestTimeout;
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_networkRequest;
    int m_downloadsIndex;
    DownloadsList m_downloads;

    void doUrlDownload(QString url);
};

#endif // PROGRESSDIALOG_H
