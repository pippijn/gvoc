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
QSgml::findEndComment (const QString &sgmlString, int &iPos)
{
  iPos = sgmlString.indexOf ("-->", iPos);
}

// create a html-file as string
QString
QSgml::toString (char Optimze, int Tabsize)
{
  QString HtmlString;
  int iLevelDiff;
  int iDummy;
  QSgmlTag::TagType eLastType;
  QSgmlTag *pTag = docTag->children[0];

  QList<QSgmlTag *> StartTags;
  QString sAtr;
  QString sSpc;
  QString sTab = "";
  QString sWrap = "";
  QHash<QString, QString>::iterator i;

  // set new-line and tab
  if (Optimze > 0)
    {
      int i;
      sWrap = "\n";
      // create the tab in spaces
      for (i = 0; i < Tabsize; i++)
        sTab += " ";
    }

  while (pTag->type != QSgmlTag::VirtualEndTag)
    {
      // create spaces in front
      for (int j = 0; j < pTag->level - 1; j++)
        sSpc += sTab;

      switch (pTag->type)
        {
        case QSgmlTag::VirtualBeginTag:
        case QSgmlTag::VirtualEndTag:
          // nothing to do
          break;
        case QSgmlTag::CDATA:
          HtmlString.append (pTag->value);
          break;
        case QSgmlTag::Comment:
          HtmlString.append (sSpc + "<!--" + pTag->value + "-->");
          break;
        case QSgmlTag::DocType:
          HtmlString.append (sSpc + "<!" + pTag->value + ">");
          break;
        case QSgmlTag::EndTag:
          HtmlString.append (sSpc + "</" + pTag->name + ">");
          break;
        case QSgmlTag::Standalone:
          sAtr = "";
          for (i = pTag->attributes.begin (); i != pTag->attributes.end (); ++i)
            {
              QString sKey = i.key ();
              sAtr += " " + sKey + "=\"" + pTag->attributes.value (sKey) + "\"";
            }
          HtmlString.append (sSpc + "<" + pTag->name + sAtr + "/>");
          break;
        case QSgmlTag::StartTag:
          sAtr = "";
          for (i = pTag->attributes.begin (); i != pTag->attributes.end (); ++i)
            {
              QString sKey = i.key ();
              sAtr += " " + sKey + "=\"" + pTag->attributes.value (sKey) + "\"";
            }
          HtmlString.append (sSpc + "<" + pTag->name + sAtr + ">");
          StartTags.append (pTag);
          break;
        case QSgmlTag::StartEmpty:
          sAtr = "";
          for (i = pTag->attributes.begin (); i != pTag->attributes.end (); ++i)
            {
              QString sKey = i.key ();
              sAtr += " " + sKey + "=\"" + pTag->attributes.value (sKey) + "\"";
            }
          HtmlString.append (sSpc + "<" + pTag->name + sAtr + "></" + pTag->name + ">");
          break;
        case QSgmlTag::NoTag:
          break;
        }

      iDummy = pTag->level;
      eLastType = pTag->type;
      pTag = &pTag->nextElement ();
      iLevelDiff = iDummy - pTag->level;
      if (iLevelDiff > 0)
        {
          // with all end-tags
          for (int j = 0; j < iLevelDiff; j++)
            // if there are any left
            if (StartTags.count () > 0)
              {
                // create spaces in front
                if ((j != 0) || (eLastType != QSgmlTag::CDATA))
                  {
                    // last tag was an endtag or not a CDATA-tag
                    sSpc = sWrap;
                    for (int i = 0; i < StartTags.last ()->level - 1; i++)
                      sSpc += sTab;
                  }
                else // last tag was a CDATA-tag
                  sSpc = "";
                // add the tag
                HtmlString.append (sSpc + "</" + StartTags.last ()->name + ">");
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
QSgml::moveChildren (QSgmlTag *source, QSgmlTag *dest)
{
  int i;
  int iCount = source->children.count ();

  for (i = 0; i < iCount; i++)
    {
      source->children.at(0)->parent = dest;
      //Source->Children[0]->Level = Dest->Level+1;  // ToDo: Level von allen Kindern auch decrementieren!
      dest->resetLevel ();
      dest->children.append (source->children[0]);
      source->children.removeFirst ();
    }
}

// include a CDATA in to the QSgml-class
void
QSgml::handleCDATA (QString sgmlString, QSgmlTag * &lastTag, int &start, int &end, int &pos)
{
  QRegExp noWhiteSpace ("(\\S)");

  start = end + 1;
  end = pos;
  QString dummy = sgmlString.mid (start, end - start).trimmed ();
  // set tag
  if (dummy.contains (noWhiteSpace))
    {
      QSgmlTag *tag = new QSgmlTag (dummy, QSgmlTag::CDATA, lastTag);
      tag->startTagPosition = start;
      tag->startTagLength = end - start;
      tag->endTagPosition = start;
      tag->endTagLength = end - start;
      lastTag->children.append (tag);
    }
}

// include a comment in to the QSgml-class
void
QSgml::handleComment (QString sgmlString, QSgmlTag * &lastTag, int &start, int &end, int &pos)
{
  pos += 4;
  start = pos;
  findEndComment (sgmlString, pos);
  end = pos;

  QString dummy = sgmlString.mid (start, end - start).trimmed ();
  QSgmlTag *tag = new QSgmlTag (dummy, QSgmlTag::Comment, lastTag);
  tag->startTagPosition = start - 4;
  tag->startTagLength = end - start + 7;
  tag->endTagPosition = start - 4;
  tag->endTagLength = end - start + 7;
  lastTag->children.append (tag);
  end += 2;
  pos = end;
}

// include a doctype in to the QSgml-class
void
QSgml::HandleDoctype (QString SgmlString, QSgmlTag * &pLastTag, int &iStart, int &iEnd, int &iPos)
{
  QString sDummy;
  QSgmlTag *pTag;

  iStart = iPos;
  FindEnd (SgmlString, iPos);
  iEnd = iPos;
  sDummy = SgmlString.mid (iStart + 2, iEnd - iStart - 2).trimmed ();

  pTag = new QSgmlTag (sDummy, QSgmlTag::DocType, pLastTag);
  pTag->startTagPosition = iStart;
  pTag->startTagLength = iEnd - iStart + 1;
  pTag->endTagPosition = iStart;
  pTag->endTagLength = iEnd - iStart + 1;

  pLastTag->children.append (pTag);
}

// include a endtag in to the QSgml-class
void
QSgml::HandleEndTag (QString SgmlString, QSgmlTag * &pLastTag, int &iStart, int &iEnd, int &iPos, QSgmlTag *endTag)
{
  QString sDummy;
  QSgmlTag *pTag;
  QSgmlTag *pDummyTag;

  iStart = iPos;
  FindEnd (SgmlString, iPos);
  iEnd = iPos;
  sDummy = SgmlString.mid (iStart + 1, iEnd - iStart - 1).trimmed ();

  pTag = new QSgmlTag (sDummy, QSgmlTag::EndTag, pLastTag);

  // find a fitting start-tag
  pDummyTag = pLastTag;
  while ((pDummyTag->name != pTag->name) && (pDummyTag->parent != NULL))
    pDummyTag = pDummyTag->parent;
  delete pTag;

  if (pDummyTag->parent != NULL)
    {
      // start-tag found
      while (pLastTag != pDummyTag)
        {
          // all tags in between are standalone tags
          pLastTag->type = QSgmlTag::Standalone;
          moveChildren (pLastTag, pLastTag->parent);
          pLastTag = pLastTag->parent;
        }
      // set data in start-tag
      pLastTag->endTagPosition = iStart;
      pLastTag->endTagLength = iEnd - iStart + 1;
      // tags which have no children are special (script can't be a standalone-tag)
      if (pLastTag->children.count () == 0)
        pLastTag->type = QSgmlTag::StartEmpty;
      pLastTag = pLastTag->parent;
    }
  else
    {
      // no start-tag -> end
      pLastTag->children.append (endTag);
      iPos = -1;
    }
}

// include a start-tag in to the QSgml-class
void
QSgml::HandleStartTag (QString SgmlString, QSgmlTag * &pLastTag, int &iStart, int &iEnd, int &iPos)
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
      pTag = new QSgmlTag (sDummy, QSgmlTag::Standalone, pLastTag);
      pTag->startTagPosition = iStart;
      pTag->startTagLength = iEnd - iStart + 1;
      pTag->endTagPosition = iStart;
      pTag->endTagLength = iEnd - iStart + 1;
      pLastTag->children.append (pTag);
    }
  else
    {
      // this is a start-tag
      pTag = new QSgmlTag (sDummy, QSgmlTag::StartTag, pLastTag);
      pTag->startTagPosition = iStart;
      pTag->startTagLength = iEnd - iStart + 1;
      pTag->endTagPosition = iStart;
      pTag->endTagLength = iEnd - iStart + 1;
      pLastTag->children.append (pTag);
      pLastTag = pTag;
    }
}

// find an element with a defined name
QList<QSgmlTag *>
QSgml::getElementsByName (QString Name)
{
  QList<QSgmlTag *> Elements;
  QSgmlTag *Tag = docTag;

  while (Tag->type != QSgmlTag::VirtualEndTag)
    {
      if (Tag->name == Name.toLower ())
        Elements.append (Tag);
      Tag = &Tag->nextElement ();
    }
  return Elements;
}

// find an element with a defined name and an attribute
QList<QSgmlTag *>
QSgml::getElementsByName (QString Name, QString AtrName)
{
  QList<QSgmlTag *> Elements;
  QSgmlTag *Tag = docTag;

  while (Tag->type != QSgmlTag::VirtualEndTag)
    {
      if ((Tag->name == Name) && (Tag->hasAttribute (AtrName) == true))
        Elements.append (Tag);
      Tag = &Tag->nextElement ();
    }
  return Elements;
}

// find an element with a defined name and an atribte with a value
QList<QSgmlTag *>
QSgml::getElementsByName (QString Name, QString AtrName, QString AtrValue)
{
  QList<QSgmlTag *> Elements;
  QSgmlTag *Tag = docTag;

  while (Tag->type != QSgmlTag::VirtualEndTag)
    {
      if ((Tag->name == Name) && (Tag->hasAttribute (AtrName) == true) && (Tag->attributes.value (AtrName) == AtrValue))
        Elements.append (Tag);
      Tag = &Tag->nextElement ();
    }
  return Elements;
}

// find an element with a defined attribute-name
QList<QSgmlTag *>
QSgml::getElementsByAttribute (QString AtrName)
{
  QList<QSgmlTag *> Elements;
  QSgmlTag *Tag = docTag;

  while (Tag->type != QSgmlTag::VirtualEndTag)
    {
      if (Tag->attributes.find (AtrName) != Tag->attributes.end ())
        Elements.append (Tag);
      Tag = &Tag->nextElement ();
    }
  return Elements;
}

// find an element with a defined attribute-name and attribute-value
QList<QSgmlTag *>
QSgml::getElementsByAttribute (QString AtrName, QString AtrValue)
{
  QList<QSgmlTag *> Elements;
  QSgmlTag *Tag = docTag;

  while (Tag->type != QSgmlTag::VirtualEndTag)
    {
      if (Tag->attributes.find (AtrName) != Tag->attributes.end ())
        if (Tag->attributes[AtrName] == AtrValue)
          Elements.append (Tag);

      Tag = &Tag->nextElement ();
    }
  return Elements;
}

// load from a file
bool
QSgml::load (QString sFileName)
{
  QFile fileText (QDir::cleanPath (sFileName));
  bool qExists = fileText.exists ();

  // delete old elements
  delete docTag;
  //delete EndTag;
  // create new doc-tag
  docTag = new QSgmlTag ("DocTag", QSgmlTag::VirtualBeginTag, NULL);
  endTag = new QSgmlTag ("EndTag", QSgmlTag::VirtualEndTag, docTag);
  // set EndTag as only Child of DocTag
  docTag->children.append (endTag);

  // read the file
  if (qExists == true)
    {
      fileText.open (QIODevice::ReadOnly);
      sgmlString = fileText.readAll ();
      dirPath = QFileInfo (sFileName).absoluteDir ();
    }

  // create elements
  fromString (sgmlString);

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
      s64_Count = fileText.write (sgmlString.toLocal8Bit ());
      dirPath = QFileInfo (sFileName).absoluteDir ();
    }

  if (s64_Count >= 0)
    return true;
  else
    return false;
}

// Constructor
QSgml::QSgml (void)
{
  // create DocTag and EndTag
  docTag = new QSgmlTag ("DocTag", QSgmlTag::VirtualBeginTag, NULL);
  endTag = new QSgmlTag ("EndTag", QSgmlTag::VirtualEndTag, docTag);
  // set EndTag as only Child of DocTag
  docTag->children.append (endTag);
  // Set Path to Default-Path
  dirPath = QDir::homePath ();

  // should be done externaly later
  tagExceptions.append ("script");
  tagExceptions.append ("style");
}

// Constructor
QSgml::QSgml (const QString &SgmlString)
{
  // create DocTag and EndTag
  docTag = new QSgmlTag ("DocTag", QSgmlTag::VirtualBeginTag, NULL);
  endTag = new QSgmlTag ("EndTag", QSgmlTag::VirtualEndTag, docTag);
  // set EndTag as only Child of DocTag
  docTag->children.append (endTag);
  // Set Path to Default-Path
  dirPath = QDir::homePath ();

  // should be done externaly later
  tagExceptions.append ("script");
  tagExceptions.append ("style");

  // Additional set string
  fromString (SgmlString);
}

// Constructor
QSgml::QSgml (QFile &SgmlFile)
{
  // create DocTag and EndTag
  docTag = new QSgmlTag ("DocTag", QSgmlTag::VirtualBeginTag, NULL);
  endTag = new QSgmlTag ("EndTag", QSgmlTag::VirtualEndTag, docTag);
  // set EndTag as only Child of DocTag
  docTag->children.append (endTag);
  // Set Path to Path
  dirPath = QFileInfo (SgmlFile).dir ();

  // should be done externaly later
  tagExceptions.append ("script");
  tagExceptions.append ("style");

  // read the file
  if (SgmlFile.exists () == true)
    {
      SgmlFile.open (QIODevice::ReadOnly);
      sgmlString = SgmlFile.readAll ();
    }

  // Additional set string
  fromString (sgmlString);
}

// convert a String to QSgml
void
QSgml::fromString (const QString &SgmlString)
{
  QSgmlTag *LastTag;
  int iPos = 0;
  int iStart, iEnd;

  QList<QString>::iterator i;

  sgmlString = SgmlString;

  docTag->children.clear ();

  LastTag = docTag;

  do
    {
      // Handle exception-tags
      for (i = tagExceptions.begin (); i != tagExceptions.end (); ++i)
        {
          QString sName = *i;
          if (LastTag->name.toLower () == sName)
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
          LastTag->children.append (endTag);
          break;
        }

      // there was CDATA
      else if (iPos > iEnd + 1)
        handleCDATA (SgmlString, LastTag, iStart, iEnd, iPos);

      // this is a comment
      if ((SgmlString.at (iPos + 1) == '!') && (SgmlString.at (iPos + 2) == '-') && (SgmlString.at (iPos + 3) == '-'))
        handleComment (SgmlString, LastTag, iStart, iEnd, iPos);
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
        HandleEndTag (SgmlString, LastTag, iStart, iEnd, iPos, endTag);
      // this is an Starttag of Standalone
      else
        HandleStartTag (SgmlString, LastTag, iStart, iEnd, iPos);
    }
  while (iPos != -1);
}

// destructor
QSgml::~QSgml (void)
{
  delete docTag;
}
