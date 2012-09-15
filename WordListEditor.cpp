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
    while (ui->wordList->rowCount() >= 1)
        ui->wordList->removeRow(0);

    foreach (Translation const &translation, translations)
    {
        int i = ui->wordList->rowCount();
        PrimaryTranslation const &primary = translation.primary;
        ui->wordList->insertRow(i);
        {
            QTableWidgetItem *item = new QTableWidgetItem(primary.sourceText);
            item->setData(Qt::UserRole, i);
            ui->wordList->setItem(i, 0, item);
        }
        ui->wordList->setItem(i, 1, new QTableWidgetItem(primary.translated));
        ui->wordList->setItem(i, 2, new QTableWidgetItem(primary.translatedPhonetic));
    }

    ui->total->setText(QString("%0 words").arg(translations.size()));
}


void WordListEditor::on_deleteItem_clicked()
{
    int deleted = 0;
    foreach (QTableWidgetItem *item, ui->wordList->selectedItems())
    {
        QVariant data = item->data(Qt::UserRole);
        if (!data.isValid())
            continue;

        int index = data.value<int>();
        translations.removeAt(index - deleted);
        deleted++;
    }

    initialiseList();
}
