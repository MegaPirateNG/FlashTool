#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QString version = "FlashTool ";
    version.append(FLASHTOOL_VERSION);
    ui->lblVersion->setText(version);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowCloseButtonHint);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
