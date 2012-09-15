/*------------------------------------------------------------------------------------------
QSGML helps to handle a SGML/HTML-File
Copyright (C) 2010  Andreas Lehmann

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

QSgml.h  Copyright (C) 2010  Andreas Lehmann
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions.
------------------------------------------------------------------------------------------*/ 

#ifndef QSGML_H
#define QSGML_H

#include <QString>
#include <QRegExp>
#include <QList>
#include <QFile>
#include <QDir>
#include <QSgmlTag.h>

class QSgml
{
public:
   QSgmlTag *docTag;
   QSgmlTag *endTag;
   QString sgmlString;
   QStringList tagExceptions;

   QSgml(void);
   QSgml(const QString &SgmlString);
   QSgml(QFile &SgmlFile);
   ~QSgml(void);

   class iterator
   {
      private:
         QSgmlTag *pTag;

      public:
         iterator(void) : pTag(NULL) { }
         iterator(QSgmlTag *oTag) : pTag(oTag) { }
         ~iterator() {}
         iterator &operator++() { pTag=&pTag->nextElement(); return(*this); }
         iterator operator++(int) { pTag=&pTag->nextElement(); return(*this); }
         iterator &operator--() { pTag=&pTag->previousElement(); return(*this); }
         iterator operator--(int) { pTag=&pTag->previousElement(); return(*this); }
         QSgmlTag &operator*() const  { return(*pTag); }
         QSgmlTag *operator->() const  { return(pTag); }
         bool operator==(iterator &x) { if(x.pTag==this->pTag)return(true);return(false); }
         bool operator!=(iterator &x) { if(x.pTag!=this->pTag)return(true);return(false); }
   };

   iterator begin()
   {
       return iterator(&docTag->nextElement());
   }

   iterator end()
   {
       return iterator(endTag);
   }

   bool load(QString sFileName);
   bool save(QString sFileName);

   QList<QSgmlTag *> getElementsByName(QString Name);
   QList<QSgmlTag *> getElementsByName(QString Name,QString AtrName);
   QList<QSgmlTag *> getElementsByName(QString Name,QString AtrName,QString AtrValue);
   QList<QSgmlTag *> getElementsByAttribute(QString AtrName);
   QList<QSgmlTag *> getElementsByAttribute(QString AtrName,QString AtrValue);

   QString toString(char Optimze = 2, int Tabsize = 2);
   void fromString(const QString &SgmlString);
protected:
   QDir dirPath;

   static void moveChildren(QSgmlTag *Source, QSgmlTag *dest);
   static void FindStart(const QString &HtmlString,int &iPos);
   static void FindEnd(const QString &HtmlString,int &iPos);
   static void findEndComment(const QString &sgmlString, int &iPos);
   static void handleCDATA(QString sgmlString, QSgmlTag* &lastTag, int &start, int &end, int &pos);
   static void handleComment(QString sgmlString, QSgmlTag* &lastTag, int &start, int &end, int &pos);

   static void HandleDoctype(QString SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos);
   static void HandleEndTag(QString SgmlString, QSgmlTag* &pLastTag, int &iStart, int &iEnd, int &iPos, QSgmlTag *endTag);
   static void HandleStartTag(QString SgmlString,QSgmlTag* &pLastTag,int &iStart,int &iEnd,int &iPos);
};

#endif // QSGML_H
