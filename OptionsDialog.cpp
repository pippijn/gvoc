#include "OptionsDialog.h"
#include "ui_OptionsDialog.h"

OptionsDialog::OptionsDialog(int level, QString sourceLanguage, QString targetLanguage, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->level->setValue(level);
    ui->sourceLanguage->setLanguage(sourceLanguage);
    ui->targetLanguage->setLanguage(targetLanguage);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}


int OptionsDialog::level() const
{
    return ui->level->value();
}

QString OptionsDialog::sourceLanguage() const
{
    return ui->sourceLanguage->languageCode();
}

QString OptionsDialog::targetLanguage() const
{
    return ui->targetLanguage->languageCode();
}
