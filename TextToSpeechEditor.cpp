#include "TextToSpeechEditor.h"
#include "ui_TextToSpeechEditor.h"

#include <QDebug>

TextToSpeechEditor::TextToSpeechEditor(QMap<QString, QByteArray> soundFiles, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TextToSpeechEditor)
    , sounds(soundFiles)
{
    ui->setupUi(this);
    initialiseList();
}

TextToSpeechEditor::~TextToSpeechEditor()
{
    delete ui;
}


QMap<QString, QByteArray> TextToSpeechEditor::soundFiles() const
{
    return sounds;
}


void TextToSpeechEditor::initialiseList()
{
    while (ui->wordList->rowCount() >= 1)
        ui->wordList->removeRow(0);

    int totalSize = 0;
    foreach (QString const &word, sounds.keys())
    {
        int i = ui->wordList->rowCount();
        ui->wordList->insertRow(i);

        QTableWidgetItem *item = new QTableWidgetItem(word);
        item->setData(Qt::UserRole, word);
        ui->wordList->setItem(i, 0, item);

        int size = sounds[word].size();
        ui->wordList->setItem(i, 1, new QTableWidgetItem(QString("%0").arg(size)));

        totalSize += size;
    }

    ui->total->setText(QString("%0 sounds, %1 MiB")
                           .arg(sounds.size())
                           .arg(qRound(totalSize / 1024.0 / 1024.0 * 100.0) / 100.0));
}


void TextToSpeechEditor::on_deleteItem_clicked()
{
    foreach (QTableWidgetItem *item, ui->wordList->selectedItems())
    {
        QVariant data = item->data(Qt::UserRole);
        if (!data.isValid())
            continue;

        QString word = data.value<QString>();
        sounds.remove(word);
    }

    initialiseList();
}
