#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

class LanguageManager;

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OptionsDialog(int minLevel, int maxLevel, QString sourceLanguage, QString targetLanguage, LanguageManager const &languageManager, QWidget *parent = 0);
    ~OptionsDialog();
    
    int minLevel() const;
    int maxLevel() const;
    QString sourceLanguage() const;
    QString targetLanguage() const;

private slots:
    void on_minLevel_valueChanged(int value);
    void on_maxLevel_valueChanged(int value);

private:
    Ui::OptionsDialog *ui;
};

#endif // OPTIONSDIALOG_H
