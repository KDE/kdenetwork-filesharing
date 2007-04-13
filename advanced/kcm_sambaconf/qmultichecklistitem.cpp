/***************************************************************************
                          qmultichecklistitem.cpp  -  description
                             -------------------
    begin                : Sun Jan 26 2003
    copyright            : (C) 2003 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                             *
 *  This file is part of KSambaPlugin.                                        *
 *                                                                            *
 *  KSambaPlugin is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KSambaPlugin is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with KSambaPlugin; if not, write to the Free Software               *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA  *
 *                                                                            *
 ******************************************************************************/

#include <qpen.h>
#include <qpainter.h>
#include <q3header.h>
//Added by qt3to4:
#include <QPolygon>

#include <kdebug.h>
#include <kmessagebox.h>

#include "qmultichecklistitem.moc"
#include "qmultichecklistitem.h"

static const int BoxSize = 16;


QMultiCheckListItem::QMultiCheckListItem( Q3ListView *parent) :
  Q3ListViewItem(parent) {
}

void QMultiCheckListItem::setOn(int column, bool b) {
  if (column >= (int) checkBoxColumns.size()) {
    checkBoxColumns.resize(column*2);
    checkStates.resize(column*2);
  }

  checkStates.setBit(column,b);
  checkBoxColumns.setBit(column);
  kDebug(5009) << "setOn : " << column << endl;
  repaint();
}

bool QMultiCheckListItem::isOn(int column) {
  return checkStates.testBit(column);
}

bool QMultiCheckListItem::isDisabled(int column) {
  return disableStates.testBit(column);
}

void QMultiCheckListItem::toggle(int column) {
  if (column >= (int) checkBoxColumns.size()) {
    checkBoxColumns.resize(column*2);
    checkStates.resize(column*2);
  }

  checkBoxColumns.setBit(column);
  checkStates.toggleBit(column);
  emit stateChanged(column,checkStates.testBit(column));

  repaint();
}

void QMultiCheckListItem::setDisabled(int column, bool b) {
  if (column >= (int) disableStates.size()) {
    disableStates.resize(column*2);
  }

  disableStates.setBit(column,b);
//  KMessageBox::information(0L,QString("setDisabled"),QString("disable %1 ").arg(column));
  repaint();
}

void QMultiCheckListItem::paintCell(QPainter *p,const QColorGroup & cg, int col, int width, int align)
{

  if ( !p )
    return;

  Q3ListView *lv = listView();
  if ( !lv )
    return;

  Q3ListViewItem::paintCell(p,cg,col,width,align );

  int marg = lv->itemMargin();
//  int width = BoxSize + marg*2;
  // use a provate color group and set the text/highlighted text colors
  QColorGroup mcg = cg;

  if (checkBoxColumns.testBit(col)) {
    // Bold/Italic/use default checkboxes
    // code almost identical to QCheckListItem
    Q_ASSERT( lv ); //###
    // I use the text color of defaultStyles[0], normalcol in parent listview
//    mcg.setColor( QPalette::Text, ((StyleListView*)lv)->normalcol );
    int x = 0;
    if ( align == Qt::AlignCenter ) {
      QFontMetrics fm( lv->font() );
      x = (width - BoxSize - fm.width(text(0)))/2;
    }
    int y = (height() - BoxSize) / 2;

    if ( !isEnabled() || disableStates.testBit(col))
      p->setPen( QPen( lv->palette().color( QPalette::Disabled, QPalette::Text ), 2 ) );
    else
      p->setPen( QPen( mcg.text(), 2 ) );

    if ( isSelected() && lv->header()->mapToSection( 0 ) != 0 ) {
      p->fillRect( 0, 0, x + marg + BoxSize + 4, height(),
             mcg.brush( QPalette::Highlight ) );
      if ( isEnabled() )
          p->setPen( QPen( mcg.color( QPalette::HighlightedText), 2 ) ); // FIXME! - use defaultstyles[0].selecol. luckily not used :)
    }
    p->drawRect( x+marg, y+2, BoxSize-4, BoxSize-4 );
    x++;
    y++;
    if ( checkStates.testBit(col) ) {
      QPolygon a( 7*2 );
      int i, xx, yy;
      xx = x+1+marg;
      yy = y+5;
      for ( i=0; i<3; i++ ) {
        a.setPoint( 2*i,   xx, yy );
        a.setPoint( 2*i+1, xx, yy+2 );
        xx++; yy++;
      }
      yy -= 2;
      for ( i=3; i<7; i++ ) {
        a.setPoint( 2*i,   xx, yy );
        a.setPoint( 2*i+1, xx, yy+2 );
        xx++; yy--;
      }
      p->drawLineSegments( a );
    }

  }


}



