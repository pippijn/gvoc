#ifndef PHONETICSDIALOG_H
#define PHONETICSDIALOG_H

#include <QDialog>
#include <QVariant>

class PhoneticsManager;

namespace Ui {
class PhoneticsDialog;
}

class PhoneticsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PhoneticsDialog(PhoneticsManager &manager, QString language, QWidget *parent = 0);
    ~PhoneticsDialog();

private slots:
    void nextPhonetic();
    void on_buttonBox_accepted();

    void phoneticFailure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage);
    void phoneticSuccess(QString targetLanguage, QString targetText, QVariant userData, QString nextPhonetic);

private:
    Ui::PhoneticsDialog *ui;
    PhoneticsManager &manager;
    QString language;
    QList<QString> queue;
};

#endif // PHONETICSDIALOG_H
