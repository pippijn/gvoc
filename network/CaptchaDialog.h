#ifndef CAPTCHADIALOG_H
#define CAPTCHADIALOG_H

#include <QDialog>

namespace Ui {
class CaptchaDialog;
}

class CaptchaDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CaptchaDialog(QImage captcha, QWidget *parent = 0);
    ~CaptchaDialog();

    QString text() const;
    
private:
    Ui::CaptchaDialog *ui;
};

#endif // CAPTCHADIALOG_H
