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

// set the type of the tag
void
QSgmlTag::setType (const QString &innerTag)
{
  // Prepareation
  QString dummy = innerTag.trimmed ();

  if (dummy.at (innerTag.count () - 1) == '/')
    type = EndTag;
  else if (dummy.at (0) == '!')
    type = DocType;
  else if (dummy.at (0) == '/')
    type = Standalone;
  else
    type = StartTag;
}

// set the names and attributes of this tag
void
QSgmlTag::setNameAttributes (const QString &innerTag)
{
  // Prepareation
  QString dummy = innerTag.trimmed ();

  // --- find name of tag ---
  int endName = dummy.indexOf (" ");
  name = dummy.left (endName).trimmed ().toLower ();
  if ((name.at (0) == '!') || (name.at (0) == '/'))
    name = name.mid (1);

  // --- find attributes ---
  QString allAttr = dummy.trimmed ().mid (endName).trimmed ();

  // First get the attr-name
  endName = allAttr.indexOf ("=");
  QString attrName = allAttr.left (endName).trimmed ();
  allAttr = allAttr.mid (endName + 1).trimmed ();

  // if there is a atr-name get atr-value and next atr-name
  while (endName != -1)
    {
      endName = allAttr.indexOf ("=");
      int posQot = allAttr.indexOf ("\"");

      // Save Name of Attribute
      QString nameString = attrName.toLower ();

      if (posQot < endName)
        {
          // find end of quote and "=" afterwards
          int iDummy = allAttr.indexOf ("\"", posQot + 1);
          endName = allAttr.indexOf ("=", iDummy + 1);
        }

      QString attrString;
      if (endName == -1)  // no "=" -> all attributes
        attrString = allAttr;
      else
        {
          dummy = allAttr.left (endName);

          int StartName = dummy.lastIndexOf (QRegExp ("[ \r\n\t]"));
          attrString = allAttr.left (StartName);

          attrName = allAttr.mid (StartName, endName - StartName).trimmed ();
          allAttr = allAttr.mid (endName + 1).trimmed ();
        }

      // set attribute hash; ignore " if exist
      attrString = attrString.trimmed ();
      if (attrString.at (0) == '\"' || attrString.at (attrString.count () - 1) == '\"')
        {
          QString sDummy = attrString.mid (1, attrString.count () - 2);
          attributes[nameString] = sDummy;
        }
      else if (attrString.at (0) == '\'' || attrString.at (attrString.count () - 1) == '\'')
        {
          QString sDummy = attrString.mid (1, attrString.count () - 2);
          attributes[nameString] = sDummy;
        }
      else
        attributes[nameString] = attrString;
    }
}

// check if attribute has the value
bool
QSgmlTag::checkAttribute (QString name, QString value) const
{
  QString sValue = attributes.value (name);

  if (sValue == value)
    return true;
  else
    return false;
}

// get the next tag on the same lefel
QSgmlTag *
QSgmlTag::nextSibling (void)
{
  int i;

  for (i = 0; i < parent->children.count (); i++)
    if (parent->children[i] == this)
      break;
  if (i == parent->children.count () - 1)
    return NULL;
  else
    return parent->children[i + 1];
}

// get the previous tag on the same lefel
QSgmlTag *
QSgmlTag::previousSibling (void)
{
  int i;

  for (i = 0; i < parent->children.count (); i++)
    if (parent->children[i] == this)
      break;
  if (i == 0)
    return NULL;
  else
    return parent->children[i - 1];
}

// get the next tag
QSgmlTag &
QSgmlTag::nextElement (void)
{
  QSgmlTag *element;

  // are there children
  if (this->children.count () > 0)
    element = this->children[0];
  else
    {
      // search the next parent with sibling
      element = this;
      while (element->nextSibling () == NULL)
        element = element->parent;
      element = element->nextSibling ();
    }
  return *element;
}

// get the previous tag
QSgmlTag &
QSgmlTag::previousElement (void)
{
  QSgmlTag *element;

  // Is there a previous tag on the same level
  element = this->previousSibling ();
  if (element == NULL)
    element = this->parent;
  else
    // search the last child with no children
    while (element->children.count () != 0)
      element = element->children.last ();

  return *element;
}

// get the value of an argument
QString
QSgmlTag::getArgValue (QString Key)
{
  return attributes.value (Key);
}

// reset the level of the tag
// and all childes and childs of the childs
void
QSgmlTag::resetLevel (void)
{
  List::const_iterator i;

  level = parent->level + 1;

  for (i = children.constBegin (); i != children.constEnd (); i++)
    {
      QSgmlTag *t = *i;
      t->resetLevel ();
    }
}

// returns true if the tag has an Atribute "AtrName"
bool
QSgmlTag::hasAttribute (QString name)
{
  return attributes.contains (name);
}

// add a child of this element
QSgmlTag *
QSgmlTag::addChild (QString innerTag, TagType childType)
{
  QSgmlTag *pnewTag = new QSgmlTag;

  // don't add childs to that eDoctype
  if (this->type == DocType)
    return NULL;

  // change type of this tag if it was eStandalone
  if (type == Standalone)
    type = StartTag;

  // set name or value
  switch (childType)
    {
    case VirtualBeginTag:
    case VirtualEndTag:
    case EndTag:
    case NoTag:
      // can't add childs of this type
      return NULL;
    case StartTag:
    case Standalone:
    case DocType:
    case StartEmpty:
      pnewTag->setType (innerTag);
      pnewTag->setNameAttributes (innerTag);
      break;
    case CDATA:
    case Comment:
      pnewTag->value = innerTag;
      break;
    }
  // set Type
  pnewTag->type = childType;
  // set Level
  pnewTag->level = level + 1;
  // set Parent
  pnewTag->parent = this;

  // add the new tag
  if (level == 0)
    this->children.insert (children.count () - 1, pnewTag);
  else
    this->children.append (pnewTag);

  return pnewTag;
}

// constructor
QSgmlTag::QSgmlTag (void)
{
  parent = NULL;
}

// constructor
QSgmlTag::QSgmlTag (const QString &innerTag)
{
  setType (innerTag);
  setNameAttributes (innerTag);
}

// constructor
QSgmlTag::QSgmlTag (const QString &innerTag, TagType type, QSgmlTag *parent)
    : parent(parent)
    , type(type)
{

  if (parent == NULL)
    level = 0;
  else
    level = parent->level + 1;
  if ((type != DocType) && (type != CDATA) && (type != Comment))
    {
      setNameAttributes (innerTag);
      value = "";
    }
  else
    value = innerTag;
}

// destructor
QSgmlTag::~QSgmlTag (void)
{
  while (!children.isEmpty())
    {
      delete children.last();
      children.pop_back();
    }
}
