#ifndef DICTIONARYPERSISTER_H
#define DICTIONARYPERSISTER_H

#include <QString>
class QVariant;

class Translation;

class DictionaryPersister
{
public:
    DictionaryPersister();

    Translation fromJSON(QByteArray data, bool &ok);
    Translation fromVariant(QVariant data, bool &ok);

    QString errorString() const;

private:
    QString error;
};

#endif // DICTIONARYPERSISTER_H
