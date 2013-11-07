#include "progressdialog.h"
#include "ui_progressdialog.h"

void ProgressDialog::startDownloads(Download download)
{
    DownloadsList downloads;
    downloads<<download;
    startDownloads(downloads);
}

void ProgressDialog::startDownloads(DownloadsList downloads)
{
    if (this->m_networkManager == NULL) {
        this->m_networkManager = new QNetworkAccessManager(this);
        connect(this->m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkReplyFinished(QNetworkReply*)));
    }

    if (this->m_downloadRequestTimeout == NULL) {
        this->m_downloadRequestTimeout = new QTimer();
        connect(m_downloadRequestTimeout, SIGNAL(timeout()), this, SLOT(networkReplyTimedOut()));
    }

    this->m_downloads = downloads;
    if (this->m_downloads.count() > 0) {
        this->m_downloadsIndex = 0;
        doUrlDownload(this->m_downloads[this->m_downloadsIndex].uri);
    }
}

void ProgressDialog::doUrlDownload(QString url)
{
    this->setMaximum(100);
    this->setValue(1);
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:13.0) Gecko/20100101 Firefox/13.0");
    request.setRawHeader("Cache-Control", "no-cache");
    this->m_networkRequest = this->m_networkManager->get(request);
    m_downloadRequestTimeout->start(30000);
    connect(this->m_networkRequest, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(networkReplyDownloadProgress(qint64,qint64)));
}

void ProgressDialog::networkReplyTimedOut()
{
    this->m_downloadRequestTimeout->stop();
    this->m_networkRequest->abort();
    this->cancel();
}

void ProgressDialog::networkReplyFinished(QNetworkReply *networkReply)
{
    QVariant possibleRedirectUrl = networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QString redirectUrl = possibleRedirectUrl.toUrl().toString();
    if (!redirectUrl.isEmpty()) {
        doUrlDownload(redirectUrl);
        return;
    }
    this->m_downloadRequestTimeout->stop();
    disconnect(this->m_networkRequest, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(networkReplyDownloadProgress(qint64,qint64)));

    QString filename = QDir::tempPath() + "/flashTool." + QUuid::createUuid().toString();
    QFile *file = new QFile(filename);
    if (file->open(QIODevice::ReadWrite)) {
         file->write(networkReply->readAll());
    }
    file->close();
    this->m_downloads[this->m_downloadsIndex].tmpFile = filename;
    this->m_downloads[this->m_downloadsIndex].success = (networkReply->error() == QNetworkReply::NoError);

    this->m_downloadsIndex++;
    if (this->m_downloads.count() > this->m_downloadsIndex) {
        doUrlDownload(this->m_downloads[this->m_downloadsIndex].uri);
    } else {
        emit downloadsFinished(this->m_downloads);
    }
}

void ProgressDialog::networkReplyDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    this->setMaximum(bytesTotal);
    this->setValue(bytesReceived);
    this->m_downloadRequestTimeout->stop();
    this->m_downloadRequestTimeout->start(30000);
}
