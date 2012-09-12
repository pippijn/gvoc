#ifndef TEXTTOSPEECHEDITOR_H
#define TEXTTOSPEECHEDITOR_H

#include <QDialog>
#include <QMap>

namespace Ui {
class TextToSpeechEditor;
}

class TextToSpeechEditor : public QDialog
{
    Q_OBJECT
    
public:
    explicit TextToSpeechEditor(QMap<QString, QByteArray> soundFiles, QWidget *parent = 0);
    ~TextToSpeechEditor();

    QMap<QString, QByteArray> soundFiles() const;

private:
    void initialiseList();

private slots:
    void on_deleteItem_clicked();

private:
    Ui::TextToSpeechEditor *ui;
    QMap<QString, QByteArray> sounds;
};

#endif // TEXTTOSPEECHEDITOR_H
