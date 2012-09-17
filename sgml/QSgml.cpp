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
*  QSgml.cpp  Copyright (C) 2010  Andreas Lehmann
*  This program comes with ABSOLUTELY NO WARRANTY.
*  This is free software, and you are welcome to redistribute it
*  under certain conditions.
*  ------------------------------------------------------------------------------------------*/

#include "QSgml.h"

// find the start of a tag
void
QSgml::FindStart (const QString &HtmlString, int &iPos)
{
  iPos = HtmlString.indexOf ("<", iPos);
}

// find the end of a tag
// make sure the '>' is not in a quote
void
QSgml::FindEnd (const QString &HtmlString, int &iPos)
{
  for (; iPos < HtmlString.length (); iPos++)
    {
      // Its a tag end
      if (HtmlString.at (iPos) == '>')
        return;
      if (HtmlString.at (iPos) == '\'')
        iPos = HtmlString.indexOf ("\'", iPos + 1);
      if (HtmlString.at (iPos) == '\"')
        iPos = HtmlString.indexOf ("\"", iPos + 1);
    }
  iPos = -1;
}

// find and of a comment
void
QSgml::FindEndComment (const QString &HtmlString, int &iPos)
{
  iPos = HtmlString.indexOf ("-->", iPos);
}

// create a html-file as string
QString QSgml::toString(char Optimze, int Tabsize)
{
    QString HtmlString;
  int iLevelDiff;
  int iDummy;
  QSgmlTag::TagType eLastType;
  QSgmlTagConstPointer pTag = d.DocTag->d.Children[0];

  QList<QSgmlTagConstPointer> StartTags;
  QString sAtr;
  QString sSpc;
  QString sTab = "";
  QString sWrap = "";
  QHash<QString, QString>::const_iterator i;

  // set new-line and tab
  if (Optimze > 0)
    {
      int i;
      sWrap = "\n";
      // create the tab in spaces
      for (i = 0; i < Tabsize; i++)
        sTab += " ";
    }

  while (pTag->d.Type != QSgmlTag::eVirtualEndTag)
    {
      // create spaces in front
      for (int j = 0; j < pTag->d.Level - 1; j++)
        sSpc += sTab;

      switch (pTag->d.Type)
        {
        case QSgmlTag::eVirtualBeginTag:
        case QSgmlTag::eVirtualEndTag:
          // nothing to do
          break;
        case QSgmlTag::eCdata:
          HtmlString.append (pTag->d.Value);
          break;
        case QSgmlTag::eComment:
          HtmlString.append (sSpc + "<!--" + pTag->d.Value + "-->");
          break;
        case QSgmlTag::eDoctype:
          HtmlString.append (sSpc + "<!" + pTag->d.Value + ">");
          break;
        case QSgmlTag::eEndTag:
          HtmlString.append (sSpc + "</" + pTag->d.Name + ">");
          break;
        case QSgmlTag::eStandalone:
          sAtr = "";
          for (i = pTag->d.Attributes.begin (); i != pTag->d.Attributes.end (); ++i)
            {
              QString sKey = i.key ();
              sAtr += " " + sKey + "=\"" + pTag->d.Attributes.value (sKey) + "\"";
            }
          HtmlString.append (sSpc + "<" + pTag->d.Name + sAtr + "/>");
          break;
        case QSgmlTag::eStartTag:
          sAtr = "";
          for (i = pTag->d.Attributes.begin (); i != pTag->d.Attributes.end (); ++i)
            {
              QString sKey = i.key ();
              sAtr += " " + sKey + "=\"" + pTag->d.Attributes.value (sKey) + "\"";
            }
          HtmlString.append (sSpc + "<" + pTag->d.Name + sAtr + ">");
          StartTags.append (pTag);
          break;
        case QSgmlTag::eStartEmpty:
          sAtr = "";
          for (i = pTag->d.Attributes.begin (); i != pTag->d.Attributes.end (); ++i)
            {
              QString sKey = i.key ();
              sAtr += " " + sKey + "=\"" + pTag->d.Attributes.value (sKey) + "\"";
            }
          HtmlString.append (sSpc + "<" + pTag->d.Name + sAtr + "></" + pTag->d.Name + ">");
          break;
        case QSgmlTag::eNoTag:
          break;
        }

      iDummy = pTag->d.Level;
      eLastType = pTag->d.Type;
      pTag = pTag->getNextElement ();
      iLevelDiff = iDummy - pTag->d.Level;
      if (iLevelDiff > 0)
        {
          // with all end-tags
          for (int j = 0; j < iLevelDiff; j++)
            // if there are any left
            if (StartTags.count () > 0)
              {
                // create spaces in front
                if ((j != 0) || (eLastType != QSgmlTag::eCdata))
                  {
                    // last tag was an endtag or not a CDATA-tag
                    sSpc = sWrap;
                    for (int i = 0; i < StartTags.last ()->d.Level - 1; i++)
                      sSpc += sTab;
                  }
                else // last tag was a CDATA-tag
                  sSpc = "";
                // add the tag
                HtmlString.append (sSpc + "</" + StartTags.last ()->d.Name + ">");
                StartTags.removeLast ();
              }
        }
      // reset the space in front for the next item.
      sSpc = sWrap;
    }

  return HtmlString;
}

// move all children (with children) to an other parent
void
QSgml::MoveChildren (QSgmlTagPointer Source, QSgmlTagPointer Dest)
{
  int i;
  int iCount = Source->d.Children.count ();

  for (i = 0; i < iCount; i++)
    {
      Source->d.Children[0]->d.Parent = Dest.get();
      //Source->Children[0]->Level = Dest->Level+1;  // ToDo: Level von allen Kindern auch decrementieren!
      Dest->resetLevel ();
      Dest->d.Children.append (Source->d.Children[0]);
      Source->d.Children.removeFirst ();
    }
}

// include a CDATA in to the QSgml-class
void
QSgml::HandleCdata (QString SgmlString, QSgmlTagPointer &pLastTag, int &iStart, int &iEnd, int &iPos)
{
  QString sDummy;
  QRegExp qNoWhitSpace ("(\\S)");
  QSgmlTag *pTag;

  iStart = iEnd + 1;
  iEnd = iPos;
  sDummy = SgmlString.mid (iStart, iEnd - iStart).trimmed ();
  // set tag
  if (sDummy.contains (qNoWhitSpace))
    {
      pTag = new QSgmlTag (sDummy, QSgmlTag::eCdata, pLastTag);
      pTag->d.StartTagPos = iStart;
      pTag->d.StartTagLength = iEnd - iStart;
      pTag->d.EndTagPos = iStart;
      pTag->d.EndTagLength = iEnd - iStart;
      pLastTag->d.Children.append (pTag);
    }
}

// include a comment in to the QSgml-class
void
QSgml::HandleComment (QString SgmlString, QSgmlTagPointer &pLastTag, int &iStart, int &iEnd, int &iPos)
{
  QString sDummy;
  QSgmlTag *pTag;

  iPos += 4;
  iStart = iPos;
  FindEndComment (SgmlString, iPos);
  iEnd = iPos;

  sDummy = SgmlString.mid (iStart, iEnd - iStart).trimmed ();
  pTag = new QSgmlTag (sDummy, QSgmlTag::eComment, pLastTag);
  pTag->d.StartTagPos = iStart - 4;
  pTag->d.StartTagLength = iEnd - iStart + 7;
  pTag->d.EndTagPos = iStart - 4;
  pTag->d.EndTagLength = iEnd - iStart + 7;
  pLastTag->d.Children.append (pTag);
  iEnd += 2;
  iPos = iEnd;
}

// include a doctype in to the QSgml-class
void
QSgml::HandleDoctype (QString SgmlString, QSgmlTagPointer &pLastTag, int &iStart, int &iEnd, int &iPos)
{
  QString sDummy;
  QSgmlTag *pTag;

  iStart = iPos;
  FindEnd (SgmlString, iPos);
  iEnd = iPos;
  sDummy = SgmlString.mid (iStart + 2, iEnd - iStart - 2).trimmed ();

  pTag = new QSgmlTag (sDummy, QSgmlTag::eDoctype, pLastTag);
  pTag->d.StartTagPos = iStart;
  pTag->d.StartTagLength = iEnd - iStart + 1;
  pTag->d.EndTagPos = iStart;
  pTag->d.EndTagLength = iEnd - iStart + 1;

  pLastTag->d.Children.append (pTag);
}

// include a endtag in to the QSgml-class
void
QSgml::HandleEndTag (QString SgmlString, QSgmlTagPointer &pLastTag, int &iStart, int &iEnd, int &iPos)
{
  QString sDummy;
  QSgmlTagPointer pTag;
  QSgmlTagPointer pDummyTag;

  iStart = iPos;
  FindEnd (SgmlString, iPos);
  iEnd = iPos;
  sDummy = SgmlString.mid (iStart + 1, iEnd - iStart - 1).trimmed ();

  pTag = new QSgmlTag (sDummy, QSgmlTag::eEndTag, pLastTag);

  // find a fitting start-tag
  pDummyTag = pLastTag;
  while ((pDummyTag->d.Name != pTag->d.Name) && (pDummyTag->d.Parent != NULL))
    pDummyTag = pDummyTag->d.Parent;

  if (pDummyTag->d.Parent != NULL)
    {
      // start-tag found
      while (pLastTag != pDummyTag)
        {
          // all tags in between are standalone tags
          pLastTag->d.Type = QSgmlTag::eStandalone;
          MoveChildren (pLastTag, pLastTag->d.Parent);
          pLastTag = pLastTag->d.Parent;
        }
      // set data in start-tag
      pLastTag->d.EndTagPos = iStart;
      pLastTag->d.EndTagLength = iEnd - iStart + 1;
      // tags which have no children are special (script can't be a standalone-tag)
      if (pLastTag->d.Children.count () == 0)
        pLastTag->d.Type = QSgmlTag::eStartEmpty;
      pLastTag = pLastTag->d.Parent;
    }
  else
    {
      // no start-tag -> end
      pLastTag->d.Children.append (d.EndTag);
      iPos = -1;
    }
}

// include a start-tag in to the QSgml-class
void
QSgml::HandleStartTag (QString SgmlString, QSgmlTagPointer &pLastTag, int &iStart, int &iEnd, int &iPos)
{
  QString sDummy;
  QSgmlTag *pTag;

  iStart = iPos;
  FindEnd (SgmlString, iPos);
  iEnd = iPos;
  sDummy = SgmlString.mid (iStart + 1, iEnd - iStart - 1).trimmed ();

  if (SgmlString.at (iEnd - 1) == '/')
    {
      // this is a standalone-tag
      sDummy = sDummy.left (sDummy.count () - 1);
      pTag = new QSgmlTag (sDummy, QSgmlTag::eStandalone, pLastTag);
      pTag->d.StartTagPos = iStart;
      pTag->d.StartTagLength = iEnd - iStart + 1;
      pTag->d.EndTagPos = iStart;
      pTag->d.EndTagLength = iEnd - iStart + 1;
      pLastTag->d.Children.append (pTag);
    }
  else
    {
      // this is a start-tag
      pTag = new QSgmlTag (sDummy, QSgmlTag::eStartTag, pLastTag);
      pTag->d.StartTagPos = iStart;
      pTag->d.StartTagLength = iEnd - iStart + 1;
      pTag->d.EndTagPos = iStart;
      pTag->d.EndTagLength = iEnd - iStart + 1;
      pLastTag->d.Children.append (pTag);
      pLastTag = pTag;
    }
}

// find an element with a defined name
QList<QSgmlTagConstPointer> QSgml::getElementsByName(QString Name) const
{
    QList<QSgmlTagConstPointer> Elements;
    QSgmlTagConstPointer Tag = d.DocTag;

    while (Tag->d.Type != QSgmlTag::eVirtualEndTag)
    {
        if (Tag->d.Name == Name.toLower())
            Elements.append (Tag);
        Tag = Tag->getNextElement();
    }
    return Elements;
}

// find an element with a defined name and an atribte
QList<QSgmlTagConstPointer> QSgml::getElementsByName(QString Name, QString AtrName) const
{
    QList<QSgmlTagConstPointer> Elements;
    QSgmlTagConstPointer Tag = d.DocTag;

    while (Tag->d.Type != QSgmlTag::eVirtualEndTag)
    {
        if ((Tag->d.Name == Name) && (Tag->hasAttribute (AtrName) == true))
            Elements.append (Tag);
        Tag = Tag->getNextElement ();
    }
    return Elements;
}

// find an element with a defined name and an atribte with a value
QList<QSgmlTagConstPointer> QSgml::getElementsByName(QString Name, QString AtrName, QString AtrValue) const
{
    QList<QSgmlTagConstPointer> Elements;
    QSgmlTagConstPointer Tag = d.DocTag;

    while (Tag->d.Type != QSgmlTag::eVirtualEndTag)
    {
        if ((Tag->d.Name == Name) && (Tag->hasAttribute (AtrName) == true) && (Tag->d.Attributes.value (AtrName) == AtrValue))
            Elements.append (Tag);
        Tag = Tag->getNextElement ();
    }
    return Elements;
}

// find an element with a defined attribute-name
QList<QSgmlTagConstPointer> QSgml::getElementsByAttribute(QString AtrName) const
{
    QList<QSgmlTagConstPointer> Elements;
    QSgmlTagConstPointer Tag = d.DocTag;

    while (Tag->d.Type != QSgmlTag::eVirtualEndTag)
    {
        if (Tag->d.Attributes.find (AtrName) != Tag->d.Attributes.end ())
            Elements.append (Tag);
        Tag = Tag->getNextElement ();
    }
    return Elements;
}

// find an element with a defined attribute-name and attribute-value
QList<QSgmlTagConstPointer> QSgml::getElementsByAttribute(QString AtrName, QString AtrValue) const
{
    QList<QSgmlTagConstPointer> Elements;
    QSgmlTagConstPointer Tag = d.DocTag;

    while (Tag->d.Type != QSgmlTag::eVirtualEndTag)
    {
        if (Tag->d.Attributes.find (AtrName) != Tag->d.Attributes.end ())
            if (Tag->d.Attributes[AtrName] == AtrValue)
                Elements.append (Tag);

        Tag = Tag->getNextElement ();
    }
    return Elements;
}

// load from a file
bool QSgml::load(QString sFileName)
{
  QFile fileText (QDir::cleanPath (sFileName));
  bool qExists = fileText.exists ();

  // create new doc-tag
  d.DocTag = new QSgmlTag("DocTag", QSgmlTag::eVirtualBeginTag, NULL);
  d.EndTag = new QSgmlTag("EndTag", QSgmlTag::eVirtualEndTag, d.DocTag);
  // set EndTag as only Child of DocTag
  d.DocTag->d.Children.append (d.EndTag);

  // read the file
  if (qExists == true)
    {
      fileText.open (QIODevice::ReadOnly);
      d.sSgmlString = fileText.readAll ();
    }

  // create elements
  fromString(d.sSgmlString);

  return qExists;
}

// save to a file
bool
QSgml::save (QString sFileName)
{
  QFile fileText (QDir::cleanPath (sFileName));
  qint64 s64_Count = 0;

  // write to file
  if (fileText.open (QIODevice::WriteOnly) == true)
    {
      s64_Count = fileText.write (d.sSgmlString.toLocal8Bit ());
    }

  if (s64_Count >= 0)
    return true;
  else
    return false;
}

// Constructor
QSgml::QSgml()
{
  // create DocTag and EndTag
  d.DocTag = new QSgmlTag ("DocTag", QSgmlTag::eVirtualBeginTag, NULL);
  d.EndTag = new QSgmlTag ("EndTag", QSgmlTag::eVirtualEndTag, d.DocTag);
  // set EndTag as only Child of DocTag
  d.DocTag->d.Children.append (d.EndTag);
}

// Constructor
QSgml::QSgml(const QString SgmlString)
{
  // create DocTag and EndTag
  d.DocTag = new QSgmlTag("DocTag", QSgmlTag::eVirtualBeginTag, NULL);
  d.EndTag = new QSgmlTag("EndTag", QSgmlTag::eVirtualEndTag, d.DocTag);
  // set EndTag as only Child of DocTag
  d.DocTag->d.Children.append(d.EndTag);

  // Additional set string
  fromString(SgmlString);
}

// Constructor
QSgml::QSgml (QFile &SgmlFile)
{
    // create DocTag and EndTag
    d.DocTag = new QSgmlTag("DocTag", QSgmlTag::eVirtualBeginTag, NULL);
    d.EndTag = new QSgmlTag("EndTag", QSgmlTag::eVirtualEndTag, d.DocTag);
    // set EndTag as only Child of DocTag
    d.DocTag->d.Children.append (d.EndTag);

    // read the file
    if (SgmlFile.open (QIODevice::ReadOnly))
        fromString(QString::fromUtf8(SgmlFile.readAll()));
}

// convert a String to QSgml
void
QSgml::fromString(const QString SgmlString)
{
  QSgmlTagPointer LastTag;
  int iPos = 0;
  int iStart, iEnd = 0;

  d.DocTag->d.Children.clear ();

  LastTag = d.DocTag;

  do
    {
      // Handle exception-tags
      foreach (QString sName, d.tagExceptions)
        {
          if (LastTag->d.Name.toLower () == sName)
            {
              // its an exception-tag
              iPos = SgmlString.toLower ().indexOf ("</" + sName, iPos);
              iPos--;
            }
        }

      FindStart (SgmlString, iPos);
      // no new start
      if (iPos == -1)
        {
          LastTag->d.Children.append (d.EndTag);
          break;
        }

      // there was CDATA
      else if (iPos > iEnd + 1)
        HandleCdata (SgmlString, LastTag, iStart, iEnd, iPos);

      // this is a comment
      if ((SgmlString.at (iPos + 1) == '!') && (SgmlString.at (iPos + 2) == '-') && (SgmlString.at (iPos + 3) == '-'))
        HandleComment (SgmlString, LastTag, iStart, iEnd, iPos);
      // this is a PI
#if 0
      else if (SgmlString.at (iPos + 1) == '?')
        HandleDoctype (SgmlString, LastTag, iStart, iEnd, iPos);

#endif
      // this is a Doctype
      else if (SgmlString.at (iPos + 1) == '!')
        HandleDoctype (SgmlString, LastTag, iStart, iEnd, iPos);
       // this is an Endtag
      else if (SgmlString.at (iPos + 1) == '/')
        HandleEndTag (SgmlString, LastTag, iStart, iEnd, iPos);
       // this is an Starttag of Standalone
      else
        HandleStartTag (SgmlString, LastTag, iStart, iEnd, iPos);
    }
  while (iPos != -1);
}

// destructor
QSgml::~QSgml (void)
{
}
