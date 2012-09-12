#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OptionsDialog(QString sourceLanguage, QString targetLanguage, QWidget *parent = 0);
    ~OptionsDialog();
    
    QString sourceLanguage() const;
    QString targetLanguage() const;

private:
    Ui::OptionsDialog *ui;
};

#endif // OPTIONSDIALOG_H
