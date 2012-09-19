/*------------------------------------------------------------------------------------------
*  QSgmlTag helps to handle a SGML/HTML-Tag
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
*  QSgmlTag.cpp  Copyright (C) 2010  Andreas Lehmann
*  This program comes with ABSOLUTELY NO WARRANTY.
*  This is free software, and you are welcome to redistribute it
*  under certain conditions.
*  ------------------------------------------------------------------------------------------*/

#include "QSgmlTag.h"

void intrusive_ptr_add_ref(QSgmlTag const *ptr)
{
    ptr->d.refcnt++;
}

void intrusive_ptr_release(QSgmlTag const *ptr)
{
    ptr->d.refcnt--;
    if (ptr->d.refcnt == 0)
        delete ptr;
}

// set the type of the tag
void
QSgmlTag::setType (const QString &InnerTag)
{
  // Prepareation
  QString sDummy = InnerTag.trimmed ();

  if (sDummy.at (InnerTag.count () - 1) == '/')
    d.Type = eEndTag;
  else if (sDummy.at (0) == '!')
    d.Type = eDoctype;
  else if (sDummy.at (0) == '/')
    d.Type = eStandalone;
  else
    d.Type = eStartTag;
}

// set the names and attributes of this tag
void
QSgmlTag::setNameAttributes (const QString &InnerTag)
{
  QString AllAtr;
  QString AtrString;
  QString NameString;
  QString AttrName;
  QString sDummy;
  QString Atribute;
  int PosQot;
  int StartName;
  int EndName;

  // Prepareation
  sDummy = InnerTag.trimmed ();

  // --- find name of tag ---
  EndName = sDummy.indexOf (" ");
  d.Name = sDummy.left (EndName).trimmed ().toLower ();
  if ((d.Name.at (0) == '!') || (d.Name.at (0) == '/'))
    d.Name = d.Name.mid (1);

  // --- find attributes ---
  AllAtr = sDummy.trimmed ().mid (EndName).trimmed ();

  // First get the attr-name
  EndName = AllAtr.indexOf ("=");
  AttrName = AllAtr.left (EndName).trimmed ();
  AllAtr = AllAtr.mid (EndName + 1).trimmed ();

  // if there is a atr-name get atr-value and next atr-name
  while (EndName != -1)
    {
      EndName = AllAtr.indexOf ("=");
      PosQot = AllAtr.indexOf ("\"");

      // Save Name of Attribute
      NameString = AttrName.toLower ();

      if (PosQot < EndName)
        {
          // find end of quote and "=" afterwards
          int iDummy = AllAtr.indexOf ("\"", PosQot + 1);
          EndName = AllAtr.indexOf ("=", iDummy + 1);
        }
      if (EndName == -1)  // no "=" -> all attributes
        AtrString = AllAtr;
      else
        {
          sDummy = AllAtr.left (EndName);

          StartName = sDummy.lastIndexOf (QRegExp ("[ \r\n\t]"));
          AtrString = AllAtr.left (StartName);

          AttrName = AllAtr.mid (StartName, EndName - StartName).trimmed ();
          AllAtr = AllAtr.mid (EndName + 1).trimmed ();
        }

      // set attribute hash; ignore " if exist
      AtrString = AtrString.trimmed ();
      if ((AtrString.at (0) == '\"') || (AtrString.at (AtrString.count () - 1) == '\"'))
        {
          QString sDummy = AtrString.mid (1, AtrString.count () - 2);
          d.Attributes[NameString] = sDummy;
        }
      else if ((AtrString.at (0) == '\'') || (AtrString.at (AtrString.count () - 1) == '\''))
        {
          QString sDummy = AtrString.mid (1, AtrString.count () - 2);
          d.Attributes[NameString] = sDummy;
        }
      else
        d.Attributes[NameString] = AtrString;
    }
}

// check if attribute has the value
bool QSgmlTag::checkAttribute (QString AtrName, QString AtrValue) const
{
  QString sValue = d.Attributes.value (AtrName);

  if (sValue == AtrValue)
    return true;
  else
    return false;
}

// get the next tag on the same lefel
QSgmlTagConstPointer QSgmlTag::getNextSibling() const
{
  int i;

  for (i = 0; i < d.Parent->d.Children.count (); i++)
    if (d.Parent->d.Children[i] == this)
      break;
  if (i == d.Parent->d.Children.count() - 1)
    return NULL;
  else
    return d.Parent->d.Children[i + 1];
}

// get the previous tag on the same lefel
QSgmlTagConstPointer QSgmlTag::getPreviousSibling() const
{
  int i;

  for (i = 0; i < d.Parent->d.Children.count (); i++)
    if (d.Parent->d.Children[i] == this)
      break;
  if (i == 0)
    return NULL;
  else
    return d.Parent->d.Children[i - 1];
}

// get the next tag
QSgmlTagConstPointer QSgmlTag::getNextElement() const
{
  QSgmlTagConstPointer Return;

  // are there children
  if (d.Children.count () > 0)
    Return = d.Children[0];
  else
    {
      // search the next parent with sibling
      Return = this;
      while (Return->getNextSibling() == NULL)
        Return = Return->d.Parent;
      Return = Return->getNextSibling();
    }
  return Return;
}

// get the previous tag
QSgmlTagConstPointer QSgmlTag::getPreviousElement() const
{
  // Is there a previous tag on the same level
  QSgmlTagConstPointer Return = getPreviousSibling();
  if (Return == NULL)
    Return = d.Parent;
  else
    // search the last child with no children
    while (Return->d.Children.count () != 0)
      Return = Return->d.Children.last ();

  return Return;
}

// get the value of an argument
QString QSgmlTag::attribute (QString Key) const
{
  return d.Attributes.value (Key);
}

// reset the level of the tag
// and all childes and childs of the childs
void
QSgmlTag::resetLevel (void)
{
  QSgmlTagList::const_iterator i;

  d.Level = d.Parent->d.Level + 1;

  for (i = d.Children.constBegin(); i != d.Children.constEnd (); i++)
    {
      QSgmlTagPointer t = *i;
      t->resetLevel ();
    }
}

// returns true if the tag has an Atribute "AtrName"
bool QSgmlTag::hasAttribute (QString AtrName) const
{
    return d.Attributes.contains(AtrName);
}

// add a child of this element
QSgmlTagPointer QSgmlTag::addChild(QString InnerTag, TagType eType)
{
  QSgmlTagPointer pnewTag = new QSgmlTag;
  QSgmlTagPointer tagRet = pnewTag;

  // don't add childs to that eDoctype
  if (d.Type == eDoctype)
    tagRet = NULL;

  // change type of this tag if it was eStandalone
  if (d.Type == eStandalone)
    d.Type = eStartTag;

  // set name or value
  switch (eType)
    {
    case eVirtualBeginTag:
    case eVirtualEndTag:
    case eEndTag:
    case eNoTag:
      // can't add childs of this type
      tagRet = NULL;
      break;
    case eStartTag:
    case eStandalone:
    case eDoctype:
    case eStartEmpty:
      pnewTag->setType (InnerTag);
      pnewTag->setNameAttributes (InnerTag);
      break;
    case eCdata:
    case eComment:
      pnewTag->d.Value = InnerTag;
      break;
    }
  // set Type
  pnewTag->d.Type = eType;
  // set Level
  pnewTag->d.Level = d.Level + 1;
  // set Parent
  pnewTag->d.Parent = this;

  // add the new tag
  if (tagRet != NULL)
    {
      if (d.Level == 0)
        d.Children.insert (d.Children.count () - 1, pnewTag);
      else
        d.Children.append (pnewTag);
    }

  return tagRet;
}

// constructor
QSgmlTag::QSgmlTag (void)
{
    d.refcnt = 1;
    d.Parent = NULL;
}

// constructor
QSgmlTag::QSgmlTag (const QString &InnerTag)
{
    d.refcnt = 1;
    d.Parent = NULL;

    setType (InnerTag);
    setNameAttributes (InnerTag);
}

// constructor
QSgmlTag::QSgmlTag (const QString &InnerTag, TagType eType, QSgmlTagPointer tParent)
{
    d.refcnt = 1;
    d.Parent = tParent.get();
    d.Type = eType;

    if (tParent == NULL)
        d.Level = 0;
    else
        d.Level = tParent->d.Level + 1;
  if ((eType != eDoctype) && (eType != eCdata) && (eType != eComment))
    {
      setNameAttributes (InnerTag);
      d.Value = "";
    }
  else
    d.Value = InnerTag;
}

// destructor
QSgmlTag::~QSgmlTag (void)
{
}

QString QSgmlTag::toString() const
{
    QString string = "<" + d.Name;
    foreach (QString const &attr, d.Attributes.keys())
        string += QString(" %0='%1'").arg(attr, d.Attributes[attr]);
    if (d.Children.isEmpty())
        string += "/>";
    else
        string += ">...</" + d.Name + ">";
    return string;
}
