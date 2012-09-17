/*------------------------------------------------------------------------------------------
*  QSGML helps to handle a SGML/HTML-File
*  Copyright (C) 2010  Andreas Lehmann
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  QSgml.h  Copyright (C) 2010  Andreas Lehmann
*  This program comes with ABSOLUTELY NO WARRANTY.
*  This is free software, and you are welcome to redistribute it
*  under certain conditions.
*  ------------------------------------------------------------------------------------------*/

#ifndef QSGML_H
#define QSGML_H

#include <QRegExp>
#include <QStringList>
#include <QFile>
#include <QDir>

#include "QSgmlTag.h"

class QSgml
{
public:
    struct D
    {
        QSgmlTagPointer DocTag;
        QSgmlTagPointer EndTag;
        QString sSgmlString;

        // should be done externally later
        QStringList const tagExceptions;

        D()
            : tagExceptions(QStringList() << "script" << "style")
        {
        }
    } d;

    QSgml();
    QSgml(const QString SgmlString);
    QSgml(QFile &SgmlFile);
    ~QSgml();

    class const_iterator
    {
    private:
        QSgmlTagConstPointer pTag;

    public:
        const_iterator()
            : pTag(NULL)
        {
        }

        const_iterator(QSgmlTagConstPointer oTag)
            : pTag(oTag)
        {
        }

        ~const_iterator ()
        {
        }

        const_iterator &operator++()
        {
            pTag = pTag->getNextElement();
            return *this;
        }

        const_iterator operator++(int)
        {
            pTag = pTag->getNextElement();
            return *this;
        }

        const_iterator &operator--()
        {
            pTag = pTag->getPreviousElement();
            return *this;
        }

        const_iterator operator--(int)
        {
            pTag = pTag->getPreviousElement();
            return *this;
        }

        QSgmlTag const &operator*() const
        {
            return *pTag;
        }

        QSgmlTagConstPointer operator->() const
        {
            return pTag;
        }

        bool operator == (const_iterator &x)
        {
            return x.pTag == this->pTag;
        }

        bool operator!=(const_iterator &x)
        {
            return x.pTag != this->pTag;
        }
    };

  const_iterator begin() const
  {
    return const_iterator(d.DocTag->getNextElement());
  }

  const_iterator end() const
  {
    return const_iterator(d.EndTag);
  }

  bool load (QString sFileName);
  bool save (QString sFileName);

  QList<QSgmlTagConstPointer> getElementsByName(QString Name) const;
  QList<QSgmlTagConstPointer> getElementsByName(QString Name, QString AtrName) const;
  QList<QSgmlTagConstPointer> getElementsByName(QString Name, QString AtrName, QString AtrValue) const;
  QList<QSgmlTagConstPointer> getElementsByAttribute(QString AtrName) const;
  QList<QSgmlTagConstPointer> getElementsByAttribute(QString AtrName, QString AtrValue) const;

  QString toString(char Optimze = 2, int Tabsize = 2);
  void fromString (const QString SgmlString);

protected:
  void MoveChildren (QSgmlTagPointer Source, QSgmlTagPointer Dest);
  void FindStart (const QString &HtmlString, int &iPos);
  void FindEnd (const QString &HtmlString, int &iPos);
  void FindEndComment (const QString &HtmlString, int &iPos);
  void HandleCdata (QString SgmlString, QSgmlTagPointer &pLastTag, int &iStart, int &iEnd, int &iPos);
  void HandleComment (QString SgmlString, QSgmlTagPointer &pLastTag, int &iStart, int &iEnd, int &iPos);

  void HandleDoctype (QString SgmlString, QSgmlTagPointer &pLastTag, int &iStart, int &iEnd, int &iPos);
  void HandleEndTag (QString SgmlString, QSgmlTagPointer &pLastTag, int &iStart, int &iEnd, int &iPos);
  void HandleStartTag (QString SgmlString, QSgmlTagPointer &pLastTag, int &iStart, int &iEnd, int &iPos);
};

#endif // QSGML_H
