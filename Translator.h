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

    QList<Translation> wordList() const;
    void setWordList(QList<Translation> wordList);

    QMap<QString, QString> phonetics() const;
    void setPhonetics(QMap<QString, QString> phonetics);

    QString sourceLanguage() const;
    void setSourceLanguage(QString sourceLanguage);
    QString targetLanguage() const;
    void setTargetLanguage(QString targetLanguage);

    void translate(QString text);
    QString phoneticNoFetch(QString text) const;
    void phonetic(QString text, QVariant userData);

private slots:
    void translateError(QNetworkReply::NetworkError translateError);
    void translateSuccess();

    void phoneticError(QNetworkReply::NetworkError translateError);
    void phoneticSuccess();

signals:
    void translateSuccess(Translation translation);
    void translateError(QString message);
    void phoneticSuccess(QString translation, QVariant userData);
    void phoneticError(QString message);

private:
    Q_DECLARE_PRIVATE(Translator)
    TranslatorPrivate *d_ptr;
};

#endif // TRANSLATOR_H
