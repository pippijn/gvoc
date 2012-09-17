#include "PhoneticsDialog.h"
#include "ui_PhoneticsDialog.h"

#include "PhoneticsManager.h"

#include <QTimer>
#include <QFileDialog>
#include <QDebug>

PhoneticsDialog::PhoneticsDialog(PhoneticsManager &manager, QString language, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PhoneticsDialog)
    , manager(manager)
    , language(language)
{
    ui->setupUi(this);

    connect(&manager, SIGNAL(failure(QString,QString,QVariant,QString)), SLOT(phoneticFailure(QString,QString,QVariant,QString)));
    connect(&manager, SIGNAL(success(QString,QString,QVariant,QString)), SLOT(phoneticSuccess(QString,QString,QVariant,QString)));

    connect(ui->nextPhonetic, SIGNAL(clicked()), SLOT(nextPhonetic()));
}

PhoneticsDialog::~PhoneticsDialog()
{
    delete ui;
}

void PhoneticsDialog::nextPhonetic()
{
    manager.phonetic(language, queue.takeFirst(), QVariant());
}

void PhoneticsDialog::on_buttonBox_accepted()
{
    ui->phonetic->clear();

    QString text = ui->input->toPlainText();
    int newLine;
    while ((newLine = text.indexOf('\n', 0)) != -1)
    {
        queue.append(text.left(newLine));
        text.remove(0, newLine + 1);
    }

    if (!text.isEmpty())
        queue.append(text);

    nextPhonetic();
}

void PhoneticsDialog::phoneticFailure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage)
{
    qDebug() << errorMessage;
    ui->nextPhonetic->setEnabled(true);
    queue.push_front(targetText);
}

void PhoneticsDialog::phoneticSuccess(QString targetLanguage, QString targetText, QVariant userData, QString phonetic)
{
    ui->nextPhonetic->setEnabled(false);
    ui->phonetic->appendPlainText(phonetic.replace("\\n", "\n"));
    if (!queue.isEmpty())
        QTimer::singleShot(1000, this, SLOT(nextPhonetic()));
}
