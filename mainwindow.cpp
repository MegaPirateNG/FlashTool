#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().width(),this->geometry().height());
    this->m_progressDialog = new ProgressDialog();
    this->m_settings = new QSettings("MegaPirateNG", "FlashTool");

    connect(ui->btnSerialRefresh, SIGNAL(clicked()), SLOT(updateSerialPorts()));
    connect(ui->cmbPlatform, SIGNAL(currentIndexChanged(int)), SLOT(platformChanged(int)));
    connect(ui->btnFlash, SIGNAL(clicked()), SLOT(startFlash()));

    this->updateSerialPorts();
    this->updateConfigs();
}

void MainWindow::updateConfigs()
{
    connect(this->m_progressDialog, SIGNAL(downloadsFinished(DownloadsList)), this, SLOT(parseConfigs(DownloadsList)));
    this->m_progressDialog->prepare("Updating available firmwares...", false);
    this->m_progressDialog->show();
    this->m_progressDialog->startDownloads(Download(FLASHTOOL_PATH_URI));
}

void MainWindow::parseConfigs(DownloadsList downloads)
{
    Download download = downloads[0];

    disconnect(this->m_progressDialog, SIGNAL(downloadsFinished(DownloadsList)), this, SLOT(parseConfigs(DownloadsList)));

    if (!download.success) {
        QApplication::exit();
    }

    QString oldBoardType = this->m_settings->value("BoardType").toString();
    QString oldRCInput = this->m_settings->value("RCInput").toString();
    QString oldPlatform = this->m_settings->value("Platform").toString();

    int oldBoardTypeIndex = 0;
    int oldRCInputIndex = 0;
    int oldPlatformIndex = 0;

    this->m_progressDialog->prepare("Checking available firmwares...", false);

    QFile *file = new QFile(download.tmpFile);
    file->open(QIODevice::ReadOnly | QIODevice::Text);

    QXmlStreamReader xml(file);

    while (!xml.atEnd()) {
        xml.readNext();

        //Boards
        if (xml.isStartElement() && (xml.name() == "boards")) {
            ui->cmbBoardType->clear();
            while (!xml.atEnd()) {
                xml.readNext();
                if (xml.isStartElement() && (xml.name() == "board")) {
                    BoardType board;
                    board.name = xml.attributes().value("name").toString().simplified();
                    board.patch = xml.attributes().value("patch").toString().simplified();
                    board.id = xml.attributes().value("id").toString().simplified();

                    if (board.id == oldBoardType) {
                        oldBoardTypeIndex = ui->cmbBoardType->count();
                    }

                    QVariant vBoard;
                    vBoard.setValue<BoardType>(board);
                    ui->cmbBoardType->addItem(board.name, vBoard);
                }
                if (xml.isEndElement() && (xml.name() == "boards")) {
                    break;
                }
            }
        }
        //RC Inputs
        if (xml.isStartElement() && (xml.name() == "rcinputs")) {
            ui->cmbRCType->clear();
            while (!xml.atEnd()) {
                xml.readNext();
                if (xml.isStartElement() && (xml.name() == "rcinput")) {
                    RCInput input;
                    input.name = xml.attributes().value("name").toString().simplified();
                    input.patch = xml.attributes().value("patch").toString().simplified();
                    input.id = xml.attributes().value("id").toString().simplified();

                    if (input.id == oldRCInput) {
                        oldRCInputIndex = ui->cmbRCType->count();
                    }

                    QVariant vInput;
                    vInput.setValue<RCInput>(input);
                    ui->cmbRCType->addItem(input.name, vInput);
                }
                if (xml.isEndElement() && (xml.name() == "rcinputs")) {
                    break;
                }
            }
        }

        //Platforms
        if (xml.isStartElement() && (xml.name() == "platforms")) {
            ui->cmbPlatform->clear();
            while (!xml.atEnd()) {
                xml.readNext();
                if (xml.isStartElement() && (xml.name() == "platform")) {
                    Platform platform;
                    platform.name = xml.attributes().value("name").toString().simplified();
                    platform.patch = xml.attributes().value("patch").toString().simplified();
                    platform.id = xml.attributes().value("id").toString().simplified();
                    platform.image = xml.attributes().value("image").toString().simplified();
                    platform.version = xml.attributes().value("version").toString().simplified();

                    if (platform.id == oldPlatform) {
                        oldPlatformIndex = ui->cmbPlatform->count();
                    }

                    QVariant vPlatform;
                    vPlatform.setValue<Platform>(platform);
                    ui->cmbPlatform->addItem(platform.name, vPlatform);
                }
                if (xml.isEndElement() && (xml.name() == "platforms")) {
                    break;
                }
            }
        }

        //Versions
        if (xml.isStartElement() && (xml.name() == "versions")) {
            this->m_versionList.clear();
            while (!xml.atEnd()) {
                xml.readNext();
                if (xml.isStartElement() && (xml.name() == "version")) {
                    Version version;
                    version.number = xml.attributes().value("number").toString().simplified();
                    version.id = xml.attributes().value("id").toString().simplified();
                    version.source = xml.attributes().value("source").toString().simplified();
                    version.platform = xml.attributes().value("platform").toString().simplified();
                    this->m_versionList<<version;
                }
                if (xml.isEndElement() && (xml.name() == "versions")) {
                    break;
                }
            }
        }
    }

    file->close();
    file->remove();

    //Now set old values if they still exists
    ui->cmbBoardType->setCurrentIndex(oldBoardTypeIndex);
    ui->cmbRCType->setCurrentIndex(oldRCInputIndex);
    ui->cmbPlatform->setCurrentIndex(oldPlatformIndex);
    this->platformChanged(ui->cmbPlatform->currentIndex());
    this->m_progressDialog->hide();
}

void MainWindow::updateSerialPorts()
{
    ui->cmbSerialPort->clear();
    ui->cmbSerialPort->setDisabled(false);
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts()) {
        if (!info.portName.isEmpty()) {
            ui->cmbSerialPort->addItem(info.portName);
        }
    }

    if (ui->cmbSerialPort->count() == 0)
    {
        ui->cmbSerialPort->setDisabled(true);
        ui->cmbSerialPort->addItem("- no serial port found -");
    }
}

void MainWindow::platformChanged(int index)
{
    QString oldVersion = this->m_settings->value("Version").toString();
    int oldVersionIndex = 0;

    Platform platform = ui->cmbPlatform->itemData(index).value<Platform>();
    ui->lblImage->setStyleSheet("background: transparent url(:/images/resources/" + platform.image + ") center 0 no-repeat;");
    ui->cmbVersion->clear();
    ui->cmbVersion->setDisabled(false);
    for (int i = 0; i < this->m_versionList.count(); i++)
    {
        Version version = this->m_versionList[i];
        if (version.platform == platform.version)
        {
            if (version.id == oldVersion) {
                oldVersionIndex = ui->cmbVersion->count();
            }
            QVariant vVersion;
            vVersion.setValue<Version>(version);
            ui->cmbVersion->addItem(version.number, vVersion);
        }
    }

    if (ui->cmbVersion->count() == 0)
    {
        ui->cmbVersion->setDisabled(true);
        ui->cmbVersion->addItem("- no flashable version found -");
    } else {
        ui->cmbVersion->setCurrentIndex(oldVersionIndex);
    }
}

void MainWindow::startFlash()
{
    if (ui->cmbSerialPort->count() == 0)
    {
        return;
    }

    if (ui->cmbVersion->count() == 0)
    {
        return;
    }

    BoardType board = ui->cmbBoardType->itemData(ui->cmbBoardType->currentIndex()).value<BoardType>();
    RCInput rcinput = ui->cmbRCType->itemData(ui->cmbRCType->currentIndex()).value<RCInput>();
    Platform platform = ui->cmbPlatform->itemData(ui->cmbPlatform->currentIndex()).value<Platform>();
    Version version = ui->cmbVersion->itemData(ui->cmbVersion->currentIndex()).value<Version>();

    connect(this->m_progressDialog, SIGNAL(downloadsFinished(DownloadsList)), this, SLOT(prepareSourceCode(DownloadsList)));
    this->m_progressDialog->prepare("Downloading firmware " + platform.name + " (" + version.number + ") ...", true);
    this->m_progressDialog->show();

    DownloadsList downloads;
    downloads<<Download(version.source);

    if (!board.patch.isEmpty()) {
        downloads<<Download(board.patch);
    }
    if (!rcinput.patch.isEmpty()) {
        downloads<<Download(rcinput.patch);
    }
    if (!platform.patch.isEmpty()) {
        downloads<<Download(platform.patch);
    }
    this->m_progressDialog->startDownloads(downloads);
}

void MainWindow::prepareSourceCode(DownloadsList downloads)
{
    disconnect(this->m_progressDialog, SIGNAL(downloadsFinished(DownloadsList)), this, SLOT(prepareSourceCode(DownloadsList)));

    //This is just until the flashing itself is finished
    for (int i = 0; i < downloads.count(); i++) {
        QFile::remove(downloads[i].tmpFile);
    }

    this->m_progressDialog->hide();
}

MainWindow::~MainWindow()
{
    BoardType board = ui->cmbBoardType->itemData(ui->cmbBoardType->currentIndex()).value<BoardType>();
    RCInput rcinput = ui->cmbRCType->itemData(ui->cmbRCType->currentIndex()).value<RCInput>();
    Platform platform = ui->cmbPlatform->itemData(ui->cmbPlatform->currentIndex()).value<Platform>();
    Version version = ui->cmbVersion->itemData(ui->cmbVersion->currentIndex()).value<Version>();

    this->m_settings->setValue("BoardType", board.id);
    this->m_settings->setValue("RCInput", rcinput.id);
    this->m_settings->setValue("Platform", platform.id);
    this->m_settings->setValue("Version", version.id);

    delete ui;
}
