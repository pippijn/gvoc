#include "OptionsDialog.h"
#include "ui_OptionsDialog.h"

OptionsDialog::OptionsDialog(QString sourceLanguage, QString targetLanguage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->sourceLanguage->setLanguage(sourceLanguage);
    ui->targetLanguage->setLanguage(targetLanguage);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}


QString OptionsDialog::sourceLanguage() const
{
    return ui->sourceLanguage->languageCode();
}

QString OptionsDialog::targetLanguage() const
{
    return ui->targetLanguage->languageCode();
}
