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
 *  QSgmlTag.h  Copyright (C) 2010  Andreas Lehmann
 *  This program comes with ABSOLUTELY NO WARRANTY.
 *  This is free software, and you are welcome to redistribute it
 *  under certain conditions.
 *  ------------------------------------------------------------------------------------------*/

#ifndef QSGMLTAG_CPP
#define QSGMLTAG_CPP

#include <QHash>
#include <QRegExp>

class QSgmlTag
{
public:
  typedef QList<QSgmlTag *>       List;
  typedef QHash<QString, QString> QSgmlAtrHash;

  enum TagType
  {
    VirtualBeginTag,
    VirtualEndTag,
    StartTag,
    EndTag,
    Standalone,
    DocType,
    CDATA,
    Comment,
    StartEmpty,
    NoTag
  };

  int level;

  QSgmlTag *parent;
  List children;

  QSgmlAtrHash attributes;
  QString name;
  QString value;
  TagType type;
  int startTagPosition;
  int startTagLength;
  int endTagPosition;
  int endTagLength;

  QSgmlTag (void);
  QSgmlTag (const QString &innerTag);
  QSgmlTag (const QString &innerTag, TagType type, QSgmlTag *tParent);
  ~QSgmlTag (void);

  bool checkAttribute (QString name, QString value) const;
  QSgmlTag *nextSibling (void);
  QSgmlTag *previousSibling (void);
  QSgmlTag &nextElement (void);
  QSgmlTag &previousElement (void);
  QString getArgValue (QString Key);
  void resetLevel (void);
  bool hasAttribute (QString name);

  QSgmlTag *addChild (QString innerTag, TagType childType);

private:
  void setType (const QString &innerTag);
  void setNameAttributes (const QString &innerTag);
};

extern QSgmlTag NoTag;

#endif // QSGMLTAG_CPP
