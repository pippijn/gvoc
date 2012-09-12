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
    ui->wordList->clear();

    foreach (QString const &word, sounds.keys())
    {
        QListWidgetItem *item = new QListWidgetItem(QString("%0 (%1 bytes)").arg(word).arg(sounds[word].size()), ui->wordList);
        item->setData(Qt::UserRole, word);
    }
}


void TextToSpeechEditor::on_deleteItem_clicked()
{
    foreach (QListWidgetItem *item, ui->wordList->selectedItems())
    {
        QString word = item->data(Qt::UserRole).value<QString>();
        sounds.remove(word);
    }

    initialiseList();
}
