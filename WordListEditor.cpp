#include "WordListEditor.h"
#include "ui_WordListEditor.h"

#include "Translation.h"

WordListEditor::WordListEditor(QList<Translation> translations, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::WordListEditor)
    , translations(translations)
{
    ui->setupUi(this);
    initialiseList();
}

WordListEditor::~WordListEditor()
{
    delete ui;
}


QList<Translation> WordListEditor::wordList() const
{
    return translations;
}


void WordListEditor::initialiseList()
{
    ui->wordList->clear();

    int i = 0;
    foreach (Translation const &translation, translations)
    {
        Translation::Primary const &primary = translation.primary.at(0);
        QListWidgetItem *item = new QListWidgetItem(QString("%0 (%1)").arg(primary.sourceText).arg(primary.translated), ui->wordList);
        item->setData(Qt::UserRole, i);
        i++;
    }
}


void WordListEditor::on_deleteItem_clicked()
{
    int deleted = 0;
    foreach (QListWidgetItem *item, ui->wordList->selectedItems())
    {
        int index = item->data(Qt::UserRole).value<int>();
        translations.removeAt(index - deleted);
        deleted++;
    }

    initialiseList();
}
