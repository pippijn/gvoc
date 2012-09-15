#include "PhoneticsEditor.h"
#include "ui_PhoneticsEditor.h"

PhoneticsEditor::PhoneticsEditor(QMap<QString, QString> phonetics, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PhoneticsEditor)
    , phoneticMap(phonetics)
{
    ui->setupUi(this);
    initialiseList();
}

PhoneticsEditor::~PhoneticsEditor()
{
    delete ui;
}


QMap<QString, QString> PhoneticsEditor::phonetics() const
{
    return phoneticMap;
}


void PhoneticsEditor::initialiseList()
{
    while (ui->wordList->rowCount() >= 1)
        ui->wordList->removeRow(0);

    foreach (QString const &phonetic, phoneticMap.keys())
    {
        int i = ui->wordList->rowCount();
        ui->wordList->insertRow(i);

        QTableWidgetItem *item = new QTableWidgetItem(phonetic);
        item->setData(Qt::UserRole, phonetic);
        ui->wordList->setItem(i, 0, item);
        ui->wordList->setItem(i, 1, new QTableWidgetItem(phoneticMap[phonetic]));
    }

    ui->total->setText(QString("%0 phonetics").arg(phoneticMap.size()));
}


void PhoneticsEditor::on_deleteItem_clicked()
{
    foreach (QTableWidgetItem *item, ui->wordList->selectedItems())
    {
        QVariant data = item->data(Qt::UserRole);
        if (!data.isValid())
            continue;

        QString phonetic = data.value<QString>();
        phoneticMap.remove(phonetic);
    }

    initialiseList();
}
