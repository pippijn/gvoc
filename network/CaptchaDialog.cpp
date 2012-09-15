#include "CaptchaDialog.h"
#include "ui_CaptchaDialog.h"

CaptchaDialog::CaptchaDialog(QImage captcha, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CaptchaDialog)
{
    ui->setupUi(this);
    ui->image->setPixmap(QPixmap::fromImage(captcha));
}

CaptchaDialog::~CaptchaDialog()
{
    delete ui;
}


QString CaptchaDialog::text() const
{
    return ui->text->text();
}
