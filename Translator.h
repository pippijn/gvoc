#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>
#include <QNetworkReply>

#include "Translation.h"

class TranslatorPrivate;

class Translator : public QObject
{
    Q_OBJECT

public:
    Translator(QString sourceLanguage, QString targetLanguage, QObject *parent = 0);
    ~Translator();

    QMap<QString, QString> phonetics() const;
    void setPhonetics(QMap<QString, QString> phonetics);

    QString sourceLanguage() const;
    void setSourceLanguage(QString sourceLanguage);
    QString targetLanguage() const;
    void setTargetLanguage(QString targetLanguage);

    void phonetic(QString text, QVariant userData);

private slots:
    void phoneticError(QNetworkReply::NetworkError translateError);
    void phoneticSuccess();

signals:
    void phoneticSuccess(QString translation, QVariant userData);
    void phoneticError(QString message);

private:
    Q_DECLARE_PRIVATE(Translator)
    TranslatorPrivate *d_ptr;
};

#endif // TRANSLATOR_H
