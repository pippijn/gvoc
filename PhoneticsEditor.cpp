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
    ui->wordList->clear();

    int i = 0;
    foreach (QString const &phonetic, phoneticMap.keys())
    {
        QListWidgetItem *item = new QListWidgetItem(QString("%0 (%1)").arg(phonetic).arg(phoneticMap[phonetic]), ui->wordList);
        item->setData(Qt::UserRole, phonetic);
        i++;
    }
}


void PhoneticsEditor::on_deleteItem_clicked()
{
    foreach (QListWidgetItem *item, ui->wordList->selectedItems())
    {
        QString phonetic = item->data(Qt::UserRole).value<QString>();
        phoneticMap.remove(phonetic);
    }

    initialiseList();
}
