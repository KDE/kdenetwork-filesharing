/* This file is part of the KDE libraries
   Copyright (C) 2005 Waldo Bastian <bastian@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "krichtextlabel.h"
#include <QTextDocument>


#include <q3stylesheet.h>
#include <q3simplerichtext.h>
#include <qtextdocument.h>
//Added by qt3to4:
#include <QLabel>

#include <kglobalsettings.h>

static QString qrichtextify( const QString& text )
{
  if ( text.isEmpty() || text[0] == '<' )
    return text;

  QStringList lines = QStringList::split('\n', text);
  for(QStringList::Iterator it = lines.begin(); it != lines.end(); ++it)
  {
    *it = Qt::convertFromPlainText( *it, Qt::WhiteSpaceNormal );
  }

  return lines.join(QString::null);	//krazy:exclude=nullstrassign for old broken gcc
}

KRichTextLabel::KRichTextLabel( const QString &text , QWidget *parent )
 : QLabel ( parent ) {
  m_defaultWidth = qMin(400, KGlobalSettings::desktopGeometry(this).width()*2/5);
  setWordWrap( true );
  setText(text);
}

KRichTextLabel::KRichTextLabel( QWidget *parent )
 : QLabel ( parent ) {
  m_defaultWidth = qMin(400, KGlobalSettings::desktopGeometry(this).width()*2/5);
  setWordWrap( true );
}

void KRichTextLabel::setDefaultWidth(int defaultWidth)
{
  m_defaultWidth = defaultWidth;
  updateGeometry();
}

QSizePolicy KRichTextLabel::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum, false);
}

QSize KRichTextLabel::minimumSizeHint() const
{
  QString qt_text = qrichtextify( text() );
  int pref_width = 0;
  int pref_height = 0;
  QTextDocument rt;
  rt.setHtml( qt_text );

  pref_width = m_defaultWidth;
  rt.setTextWidth(pref_width);
  int used_width = rt.idealWidth();
  if (used_width <= pref_width)
  {
    while(true)
    {
      int new_width = (used_width * 9) / 10;
      rt.setTextWidth(new_width);
      int new_height = rt.size().height();
      if (new_height > pref_height)
        break;
      used_width = rt.idealWidth();
      if (used_width > new_width)
        break;
    }
    pref_width = used_width;
  }
  else
  {
    if (used_width > (pref_width *2))
      pref_width = pref_width *2;
    else
      pref_width = used_width;
  }

  return QSize(pref_width, rt.size().height());
}

QSize KRichTextLabel::sizeHint() const
{
  return minimumSizeHint();
}

void KRichTextLabel::setText( const QString &text ) {
  QLabel::setText(text);
}

void KRichTextLabel::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

#include "krichtextlabel.moc"
