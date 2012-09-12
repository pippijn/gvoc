#ifndef PHONETICSEDITOR_H
#define PHONETICSEDITOR_H

#include <QDialog>
#include <QMap>

namespace Ui {
class PhoneticsEditor;
}

class PhoneticsEditor : public QDialog
{
    Q_OBJECT
    
public:
    explicit PhoneticsEditor(QMap<QString, QString> phonetics, QWidget *parent = 0);
    ~PhoneticsEditor();

    QMap<QString, QString> phonetics() const;

private:
    void initialiseList();

private slots:
    void on_deleteItem_clicked();

private:
    Ui::PhoneticsEditor *ui;
    QMap<QString, QString> phoneticMap;
};

#endif // PHONETICSEDITOR_H
