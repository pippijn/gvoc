#include "DictionaryPersister.h"
#include "Translation.h"

#include <qjson/parser.h>

#include <QDebug>
#include <QByteArray>
#include <QVariant>


DictionaryPersister::DictionaryPersister()
{
}


Translation DictionaryPersister::fromJSON(QByteArray data, bool &ok)
{
    // Replace all empty spaces with "null". Needs to be done twice.
    data.replace(",,", ",null,");
    data.replace(",,", ",null,");
    data.replace("[,", "[null,");

    // Parse JSON text into QVariant.
    QJson::Parser parser;
    QVariant parsed = parser.parse(data, &ok);
    if (!ok)
    {
        error = parser.errorString();
        return Translation();
    }

#if 0
    qDebug() << QString::fromUtf8(data.constData(), data.size());
#endif

    return fromVariant(parsed, ok);
}


Translation DictionaryPersister::fromVariant(QVariant variant, bool &ok)
{
    ok = true;
    return Translation(variant.value<QVariantList>());
}


QString DictionaryPersister::errorString() const
{
    return error;
}
