#include "LanguageChooser.h"
#include "ui_LanguageChooser.h"

LanguageChooser::LanguageChooser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LanguageChooser)
{
    ui->setupUi(this);
}

LanguageChooser::~LanguageChooser()
{
    delete ui;
}

QString LanguageChooser::languageCode() const
{
    return lang;
}

void LanguageChooser::setLanguage(QString languageCode)
{
    if (languageCode == "zh-CN")   ui->chinese   ->setChecked(true);
    else if (languageCode == "nl") ui->dutch     ->setChecked(true);
    else if (languageCode == "en") ui->english   ->setChecked(true);
    else if (languageCode == "fr") ui->french    ->setChecked(true);
    else if (languageCode == "de") ui->german    ->setChecked(true);
    else if (languageCode == "it") ui->italian   ->setChecked(true);
    else if (languageCode == "ja") ui->japanese  ->setChecked(true);
    else if (languageCode == "no") ui->norwegian ->setChecked(true);
    else if (languageCode == "pt") ui->portuguese->setChecked(true);
    else if (languageCode == "ru") ui->russian   ->setChecked(true);
    else if (languageCode == "es") ui->spanish   ->setChecked(true);
    else if (languageCode == "sw") ui->swedish   ->setChecked(true);
    else if (languageCode == "th") ui->thai      ->setChecked(true);
    else if (languageCode == "tr") ui->turkish   ->setChecked(true);
}

void LanguageChooser::on_chinese_toggled   (bool checked) { if (checked) lang = "zh-CN"; }
void LanguageChooser::on_dutch_toggled     (bool checked) { if (checked) lang = "nl"; }
void LanguageChooser::on_english_toggled   (bool checked) { if (checked) lang = "en"; }
void LanguageChooser::on_french_toggled    (bool checked) { if (checked) lang = "fr"; }
void LanguageChooser::on_german_toggled    (bool checked) { if (checked) lang = "de"; }
void LanguageChooser::on_italian_toggled   (bool checked) { if (checked) lang = "it"; }
void LanguageChooser::on_japanese_toggled  (bool checked) { if (checked) lang = "ja"; }
void LanguageChooser::on_norwegian_toggled (bool checked) { if (checked) lang = "no"; }
void LanguageChooser::on_portuguese_toggled(bool checked) { if (checked) lang = "pt"; }
void LanguageChooser::on_russian_toggled   (bool checked) { if (checked) lang = "ru"; }
void LanguageChooser::on_spanish_toggled   (bool checked) { if (checked) lang = "es"; }
void LanguageChooser::on_swedish_toggled   (bool checked) { if (checked) lang = "sw"; }
void LanguageChooser::on_thai_toggled      (bool checked) { if (checked) lang = "th"; }
void LanguageChooser::on_turkish_toggled   (bool checked) { if (checked) lang = "tr"; }
