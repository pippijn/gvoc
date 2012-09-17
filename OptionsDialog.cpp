#include "OptionsDialog.h"
#include "ui_OptionsDialog.h"

OptionsDialog::OptionsDialog(int minLevel, int maxLevel, QString sourceLanguage, QString targetLanguage, LanguageManager const &languageManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->minLevel->setValue(minLevel);
    ui->maxLevel->setValue(maxLevel);
    ui->sourceLanguage->setLanguage(sourceLanguage);
    ui->targetLanguage->setLanguage(targetLanguage);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}


int OptionsDialog::minLevel() const
{
    return ui->minLevel->value();
}

int OptionsDialog::maxLevel() const
{
    return ui->maxLevel->value();
}

QString OptionsDialog::sourceLanguage() const
{
    return ui->sourceLanguage->languageCode();
}

QString OptionsDialog::targetLanguage() const
{
    return ui->targetLanguage->languageCode();
}

void OptionsDialog::on_minLevel_valueChanged(int value)
{
    ui->maxLevel->setValue(qMax(ui->maxLevel->value(), value));
}

void OptionsDialog::on_maxLevel_valueChanged(int value)
{
    ui->minLevel->setValue(qMin(ui->minLevel->value(), value));
}
