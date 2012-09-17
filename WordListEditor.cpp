#include "WordListEditor.h"
#include "ui_WordListEditor.h"

#include "Translation.h"
#include "WordListModel.h"

#include <QDebug>

WordListEditor::WordListEditor(QList<Translation> translations, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::WordListEditor)
    , translations(translations)
{
    ui->setupUi(this);
    ui->wordList->setModel(new WordListModel(translations, this));
}

WordListEditor::~WordListEditor()
{
    delete ui;
}


QList<Translation> WordListEditor::wordList() const
{
    return translations;
}

void WordListEditor::on_deleteItem_clicked()
{
    ui->wordList->removeSelectedRows();
}
