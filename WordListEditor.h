#ifndef WORDLISTEDITOR_H
#define WORDLISTEDITOR_H

#include <QDialog>

class Translation;

namespace Ui {
class WordListEditor;
}

class WordListEditor : public QDialog
{
    Q_OBJECT
    
public:
    explicit WordListEditor(QList<Translation> translations, QWidget *parent = 0);
    ~WordListEditor();

    QList<Translation> wordList() const;

private:
    void initialiseList();

private slots:
    void on_deleteItem_clicked();

private:
    Ui::WordListEditor *ui;
    QList<Translation> translations;
};

#endif // WORDLISTEDITOR_H
