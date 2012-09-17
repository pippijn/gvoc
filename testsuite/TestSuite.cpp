#include "TestSuite.h"

#include "QSgml.h"

#include <QDebug>

static void testSgml()
{
    QFile file(":/testsuite/test.html");
    if (!file.open(QFile::ReadOnly))
        return;
    QSgml sgml(QString::fromUtf8(file.readAll()));

    foreach (QSgmlTag const &tag, sgml)
    {
        tag.toString();
    }
}


TestSuite::TestSuite()
{
}

void TestSuite::run()
{
    testSgml();
}
