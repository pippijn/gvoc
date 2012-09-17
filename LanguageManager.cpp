#include "LanguageManager.h"

#include <QDebug>


LanguageManager::LanguageManager()
{
    addLanguage("zh-CN", "Chinese");
    addLanguage("nl"   , "Dutch");
    addLanguage("en"   , "English");
    addLanguage("fr"   , "French");
    addLanguage("de"   , "German");
    addLanguage("it"   , "Italian");
    addLanguage("ja"   , "Japanese");
    addLanguage("no"   , "Norwegian");
    addLanguage("pt"   , "Portuguese");
    addLanguage("ru"   , "Russian");
    addLanguage("es"   , "Spanish");
    addLanguage("sw"   , "Swedish");
    addLanguage("th"   , "Thai");
    addLanguage("tr"   , "Turkish");
}


QStringList const &LanguageManager::availableLanguages() const
{
    return codes;
}


QString LanguageManager::languageName(QString code) const
{
    if (names.contains(code))
        return names[code];

    qWarning() << "unknown language code:" << code;
    return "<unknown>";
}


void LanguageManager::addLanguage(QString code, QString name)
{
    codes << code;
    names[code] = name;
}
