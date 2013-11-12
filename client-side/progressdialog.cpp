#include "progressdialog.h"

ProgressDialog::ProgressDialog()
{
    this->setAutoClose(false);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowCloseButtonHint);

    this->m_networkManager = new QNetworkAccessManager(this);
    connect(this->m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkReplyFinished(QNetworkReply*)));
    this->m_downloadRequestTimeout = new QTimer();
    connect(m_downloadRequestTimeout, SIGNAL(timeout()), this, SLOT(networkReplyTimedOut()));
    connect(this, SIGNAL(canceled()), this, SLOT(onCanceled()));
}

void ProgressDialog::startDownloads(Download download)
{
    DownloadsList downloads;
    downloads<<download;
    startDownloads(downloads);
}

void ProgressDialog::startDownloads(DownloadsList downloads)
{
    this->m_downloads = downloads;
    if (this->m_downloads.count() > 0) {
        this->m_downloadsIndex = 0;
        doUrlDownload(this->m_downloads[this->m_downloadsIndex]);
    }
}

void ProgressDialog::doUrlDownload(Download download)
{
    QString userAgent = "FlashTool ";
    userAgent.append(FLASHTOOL_VERSION);
    download.tries++;
    this->setMaximum(100);
    this->setValue(1);
    QNetworkRequest request;
    request.setUrl(download.uri);
    request.setRawHeader("User-Agent", userAgent.toLatin1());
    request.setRawHeader("Cache-Control", "no-cache");
    request.setRawHeader("Content-Type", "text/xml");

    if (download.body.isEmpty()) {
        this->m_networkRequest = this->m_networkManager->get(request);
    } else {
        this->m_networkRequest = this->m_networkManager->post(request, download.body.toLatin1());
    }
    m_downloadRequestTimeout->start(30000);
    connect(this->m_networkRequest, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(networkReplyDownloadProgress(qint64,qint64)));
}

void ProgressDialog::onCanceled()
{
    this->m_downloadRequestTimeout->stop();
    this->m_networkRequest->abort();
}

void ProgressDialog::networkReplyTimedOut()
{
    emit canceled();
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
