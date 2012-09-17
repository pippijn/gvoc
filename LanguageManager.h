#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QStringList>
#include <QMap>

class LanguageManager
{
public:
    LanguageManager();

    QStringList const &availableLanguages() const;
    QString languageName(QString code) const;

private:
    void addLanguage(QString code, QString name);

private:
    QStringList codes;
    QMap<QString, QString> names;
};

#endif // LANGUAGEMANAGER_H
