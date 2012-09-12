#ifndef LANGUAGECHOOSER_H
#define LANGUAGECHOOSER_H

#include <QWidget>

namespace Ui {
class LanguageChooser;
}

class LanguageChooser : public QWidget
{
    Q_OBJECT
    
public:
    explicit LanguageChooser(QWidget *parent = 0);
    ~LanguageChooser();

    QString languageCode() const;
    void setLanguage(QString languageCode);
    
private slots:
    void on_chinese_toggled(bool checked);
    void on_dutch_toggled(bool checked);
    void on_english_toggled(bool checked);
    void on_french_toggled(bool checked);
    void on_german_toggled(bool checked);
    void on_italian_toggled(bool checked);
    void on_japanese_toggled(bool checked);
    void on_norwegian_toggled(bool checked);
    void on_portuguese_toggled(bool checked);
    void on_russian_toggled(bool checked);
    void on_spanish_toggled(bool checked);
    void on_swedish_toggled(bool checked);
    void on_thai_toggled(bool checked);
    void on_turkish_toggled(bool checked);

private:
    Ui::LanguageChooser *ui;
    QString lang;
};

#endif // LANGUAGECHOOSER_H
