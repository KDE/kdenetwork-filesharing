/***************************************************************************
                          hiddenfileview.cpp  -  description
                             -------------------
    begin                : Wed Jan 1 2003
    copyright            : (C) 2003 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KSambaPlugin.                                          *
 *                                                                            *
 *  KSambaPlugin is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KSambaPlugin is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with KSambaPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#include <assert.h>

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qgroupbox.h>
#include <qcursor.h>


#include <kpopupmenu.h>
#include <kaction.h>
#include <krestrictedline.h>
#include <klocale.h>
#include <kfiledetailview.h>
#include <kdirlister.h>
#include <kmessagebox.h>
#include <kurlrequester.h>

#include "hiddenfileview.h"
#include "sharedlgimpl.h"
#include "sambashare.h"


#define COL_NAME 0
#define COL_HIDDEN 1
#define COL_VETO 2
#define COL_SIZE 3
#define COL_DATE 4
#define COL_PERM 5
#define COL_OWNER 6
#define COL_GROUP 7

#define HIDDENTABINDEX 5

HiddenListViewItem::HiddenListViewItem( QListView *parent, KFileItem *fi, bool hidden, bool veto )
  : KListViewItem( parent )
{
  setPixmap( COL_NAME, fi->pixmap(KIcon::SizeSmall));

  setText( COL_NAME, fi->text() );
  setText( COL_SIZE, KGlobal::locale()->formatNumber( fi->size(), 0));
  setText( COL_DATE,  fi->timeString() );
  setText( COL_PERM,  fi->permissionsString() );
  setText( COL_OWNER, fi->user() );
  setText( COL_GROUP, fi->group() );

  setHidden(hidden);
  setVeto(veto);

  _fileItem = fi;
}

HiddenListViewItem::~HiddenListViewItem()
{
}

KFileItem* HiddenListViewItem::getFileItem()
{
  return _fileItem;
}

void HiddenListViewItem::setVeto(bool b)
{
  _veto = b;
  setText( COL_VETO, _veto ? i18n("Yes") : QString(""));
}

bool HiddenListViewItem::isVeto()
{
  return _veto;
}

void HiddenListViewItem::setHidden(bool b)
{
  _hidden = b;
  setText( COL_HIDDEN, _hidden ? i18n("Yes") : QString("") );
}

bool HiddenListViewItem::isHidden()
{
  return _hidden;
}

void HiddenListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
  QColorGroup _cg = cg;

  if (isVeto())
     _cg.setColor(QColorGroup::Base,lightGray);

  if (isHidden())
     _cg.setColor(QColorGroup::Text,gray);

  QListViewItem::paintCell(p, _cg, column, width, alignment);
}




HiddenFileView::HiddenFileView(ShareDlgImpl* shareDlg, SambaShare* share)
{
  _share = share;
  _dlg = shareDlg;

  _hiddenActn = new KToggleAction(i18n("&Hide"));
  _vetoActn = new KToggleAction(i18n("&Veto"));

  initListView();

  _dlg->hiddenChk->setTristate(true);
  _dlg->vetoChk->setTristate(true);

  connect( _dlg->hiddenChk, SIGNAL(toggled(bool)), this, SLOT( hiddenChkClicked(bool) ));
  connect( _dlg->vetoChk, SIGNAL(toggled(bool)), this, SLOT( vetoChkClicked(bool) ));

  _dlg->hiddenEdit->setText( _share->getValue("hide files") );
  connect( _dlg->hiddenEdit, SIGNAL(textChanged(const QString &)), this, SLOT(updateView()));

  _dlg->vetoEdit->setText( _share->getValue("veto files") );
  connect( _dlg->vetoEdit, SIGNAL(textChanged(const QString &)), this, SLOT(updateView()));


//  new QLabel(i18n("Hint")+" : ",grid);
//  new QLabel(i18n("You have to separate the entries with a '/'. You can use the wildcards '*' and '?'"),grid);
//  new QLabel(i18n("Example")+" : ",grid);
//  new QLabel(i18n("*.tmp/*SECRET*/.*/file?.*/"),grid);

  _dir = new KDirLister(true);
  _dir->setShowingDotFiles(true);
  
  connect( _dir, SIGNAL(newItems(const KFileItemList &)),
           this, SLOT(insertNewFiles(const KFileItemList &)));

  connect( _dir, SIGNAL(deleteItem(KFileItem*)),
           this, SLOT(deleteItem(KFileItem*)));

  connect( _dir, SIGNAL(refreshItems(const KFileItemList &)),
           this, SLOT(refreshItems(const KFileItemList &)));
           
  connect( _hiddenActn, SIGNAL(toggled(bool)), this, SLOT(hiddenChkClicked(bool)));
  connect( _vetoActn, SIGNAL(toggled(bool)), this, SLOT(vetoChkClicked(bool)));
}

void HiddenFileView::initListView()
{
  _dlg->hiddenListView->setMultiSelection(true);
  _dlg->hiddenListView->setSelectionMode(QListView::Extended);
  _dlg->hiddenListView->setAllColumnsShowFocus(true);

  _hiddenList = createRegExpList(_share->getValue("hide files"));
  _vetoList = createRegExpList(_share->getValue("veto files"));

  _popup = new KPopupMenu(_dlg->hiddenListView);
  _hiddenActn->plug(_popup);
  _vetoActn->plug(_popup);

  connect( _dlg->hiddenListView, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  connect( _dlg->hiddenListView, SIGNAL(contextMenu(KListView*,QListViewItem*,const QPoint&)),
           this, SLOT(showContextMenu()));

  connect( _dlg->hideDotFilesChk, SIGNAL(toggled(bool)), this, SLOT(hideDotFilesChkClicked(bool)));
  connect( _dlg->hideUnreadableChk, SIGNAL(toggled(bool)), this, SLOT(hideUnreadableChkClicked(bool)));
}

HiddenFileView::~HiddenFileView()
{
}

void HiddenFileView::load()
{
  if (_dlg->hiddenListView)
    _dlg->hiddenListView->clear();
    
  _dir->openURL( _dlg->pathUrlRq->url() );
}

void HiddenFileView::save()
{
  QString s = _dlg->hiddenEdit->text().stripWhiteSpace();
  // its important that the string ends with an '/'
  // otherwise Samba won't recognize the last entry
  if (s != "" && s.right(1)!="/")
      s+="/";
  _share->setValue("hide files", s);

  s = _dlg->vetoEdit->text().stripWhiteSpace();
  // its important that the string ends with an '/'
  // otherwise Samba won't recognize the last entry
  if (s != "" && s.right(1)!="/")
      s+="/";
  _share->setValue("veto files", s);
}

void HiddenFileView::insertNewFiles(const KFileItemList &newone)
{
  if ( newone.isEmpty() )
     return;

  KFileItem *tmp;

  int j=0;

  for (KFileItemListIterator it(newone); (tmp = it.current()); ++it)
  {
    j++;

    bool hidden = matchHidden(tmp->text());
    bool veto = matchVeto(tmp->text());

    HiddenListViewItem *item = new HiddenListViewItem( _dlg->hiddenListView, tmp, hidden, veto );
  }
}

void HiddenFileView::deleteItem( KFileItem *fileItem )
{
  HiddenListViewItem* item;
  for (item = static_cast<HiddenListViewItem*>(_dlg->hiddenListView->firstChild());item;
       item = static_cast<HiddenListViewItem*>(item->nextSibling()))
  {
    if (item->getFileItem() == fileItem)
    {
      delete item;
      break;
    }
  }
    
}

void HiddenFileView::refreshItems( const KFileItemList& items )
{
  updateView();  
}


void HiddenFileView::showContextMenu()
{
  _popup->exec(QCursor::pos());
}


void HiddenFileView::selectionChanged()
{
  bool veto = false;
  bool noVeto = false;
  bool hide = false;
  bool noHide = false;

  int n = 0;

  HiddenListViewItem* item;
  for (item = static_cast<HiddenListViewItem*>(_dlg->hiddenListView->firstChild());item;
       item = static_cast<HiddenListViewItem*>(item->nextSibling()))
  {
    if (!item->isSelected())
       continue;

    n++;

    if (item->isVeto())
       veto = true;
    else
       noVeto = true;

    if (item->isHidden())
       hide = true;
    else
       noHide = true;
  }


  _dlg->selGrpBx->setEnabled(n>0);

  if (veto && noVeto)
  {
    _dlg->vetoChk->setTristate(true);
    _dlg->vetoChk->setNoChange();
    _dlg->vetoChk->update();
  }
  else
  {
    _dlg->vetoChk->setTristate(false);
    _dlg->vetoChk->setChecked(veto);
  }

  if (hide && noHide)
  {
    _dlg->hiddenChk->setTristate(true);
    _dlg->hiddenChk->setNoChange();
    _dlg->hiddenChk->update();
  }
  else
  {
    _dlg->hiddenChk->setTristate(false);
    _dlg->hiddenChk->setChecked(hide);
  }
}

void HiddenFileView::hiddenChkClicked(bool b)
{
  // We don't save the old state so
  // disable the tristate mode
  _dlg->hiddenChk->setTristate(false);
  _hiddenActn->setChecked(b);
  _dlg->hiddenChk->setChecked(b);

  HiddenListViewItem* item;
  for (item = static_cast<HiddenListViewItem*>(_dlg->hiddenListView->firstChild());item;
       item = static_cast<HiddenListViewItem*>(item->nextSibling()))
  {
    if (item->isSelected())
    {
      // If we remove a file from the list
      // perhaps it was a wildcard string
      if (!b && item->isHidden())
      {
        QRegExp* rx = getHiddenMatch(item->text(0));
        assert(rx);

        QString p = rx->pattern();
        if ( p.find("*") > -1 ||
             p.find("?") > -1 )
        {
          int result = KMessageBox::questionYesNo(_dlg,i18n(
            "<b></b>Some files you have selected are matched by the wildcarded string <b>'%1'</b>! "
            "Do you want to unhide all files matching <b>'%1'</b>? <br>"
            "(If you say no, no file matching '%1' will be unhidden)").arg(rx->pattern()).arg(rx->pattern()).arg(rx->pattern()),
            i18n("Wildcarded String"));

          QPtrList<HiddenListViewItem> lst = getMatchingItems( *rx );

          if (result == KMessageBox::No)
          {
            deselect(lst);
            continue;
          }
          else
          {
            setHidden(lst,false);
          }
        }

        _hiddenList.remove(rx);
      }
      else
      if (b && !item->isHidden())
      {
        _hiddenList.append( new QRegExp(item->text(0)) );
      }

      item->setHidden(b);
    }
  }

  updateEdit(_dlg->hiddenEdit, _hiddenList);
  _dlg->hiddenListView->update();
}

void HiddenFileView::vetoChkClicked(bool b)
{
  // We don't save the old state so
  // disable the tristate mode
  _dlg->vetoChk->setTristate(false);
  _vetoActn->setChecked(b);
  _dlg->vetoChk->setChecked(b);

  HiddenListViewItem* item;
  for (item = static_cast<HiddenListViewItem*>(_dlg->hiddenListView->firstChild());item;
       item = static_cast<HiddenListViewItem*>(item->nextSibling()))
  {
    if (item->isSelected())
    {
      // If we remove a file from the list
      // perhaps it was a wildcard string
      if (!b && item->isVeto())
      {
        QRegExp* rx = getVetoMatch(item->text(0));
        assert(rx);

        QString p = rx->pattern();
        if ( p.find("*") > -1 ||
             p.find("?") > -1 )
        {
          int result = KMessageBox::questionYesNo(_dlg,i18n(
            "<b></b>Some files you have selected are matched by the wildcarded string <b>'%1'</b>! "
            "Do you want to unveto all files matching <b>'%1'</b>? <br>"
            "(If you say no, no file matching '%1' will be unvetoed)").arg(rx->pattern()).arg(rx->pattern()).arg(rx->pattern()),
            i18n("Wildcarded String"));

          QPtrList<HiddenListViewItem> lst = getMatchingItems( *rx );

          if (result == KMessageBox::No)
          {
            deselect(lst);
            continue;
          }
          else
          {
            setVeto(lst,false);
          }
        }

        _vetoList.remove(rx);
      }
      else
      if (b && !item->isVeto())
      {
        _vetoList.append( new QRegExp(item->text(0)) );
      }

      item->setVeto(b);
    }


  }

  updateEdit(_dlg->vetoEdit, _vetoList);
  _dlg->hiddenListView->update();
}

/**
 * Sets the text of the QLineEdit edit to the entries of the passed QRegExp-List
 **/
void HiddenFileView::updateEdit(QLineEdit* edit, QPtrList<QRegExp> & lst)
{
  QString s="";

  QRegExp* rx;
  for(rx = static_cast<QRegExp*>(lst.first()); rx;
      rx = static_cast<QRegExp*>(lst.next()) )
  {
    s+= rx->pattern()+QString("/");
  }

  edit->setText(s);
}

void HiddenFileView::setVeto(QPtrList<HiddenListViewItem> & lst, bool b)
{
  HiddenListViewItem* item;
  for(item = static_cast<HiddenListViewItem*>(lst.first()); item;
      item = static_cast<HiddenListViewItem*>(lst.next()) )
  {
    item->setVeto(b);
  }
}

void HiddenFileView::setHidden(QPtrList<HiddenListViewItem> & lst, bool b)
{
  HiddenListViewItem* item;
  for(item = static_cast<HiddenListViewItem*>(lst.first()); item;
      item = static_cast<HiddenListViewItem*>(lst.next()) )
  {
    item->setHidden(b);
  }
}

void HiddenFileView::deselect(QPtrList<HiddenListViewItem> & lst)
{
  HiddenListViewItem* item;
  for(item = static_cast<HiddenListViewItem*>(lst.first()); item;
      item = static_cast<HiddenListViewItem*>(lst.next()) )
  {
    item->setSelected(false);
  }
}


QPtrList<HiddenListViewItem> HiddenFileView::getMatchingItems(const QRegExp & rx)
{
  QPtrList<HiddenListViewItem> lst;

  HiddenListViewItem* item;
  for (item = static_cast<HiddenListViewItem*>(_dlg->hiddenListView->firstChild());item;
       item = static_cast<HiddenListViewItem*>(item->nextSibling()))
  {
    if (rx.exactMatch(item->text(0)))
       lst.append(item);
  }

  return lst;
}

void HiddenFileView::updateView()
{
  _hiddenList = createRegExpList(_dlg->hiddenEdit->text());
  _vetoList = createRegExpList(_dlg->vetoEdit->text());

  HiddenListViewItem* item;
  for (item = static_cast<HiddenListViewItem*>(_dlg->hiddenListView->firstChild());item;
       item = static_cast<HiddenListViewItem*>(item->nextSibling()))
  {
    item->setHidden(matchHidden(item->text(0)));
    item->setVeto(matchVeto(item->text(0)));
  }

  _dlg->hiddenListView->repaint();
}


QPtrList<QRegExp> HiddenFileView::createRegExpList(const QString & s)
{
  QPtrList<QRegExp> lst;
  bool cs = _share->getBoolValue("case sensitive");

  if (s != "")
  {
    QStringList l = QStringList::split("/",s);

    for ( QStringList::Iterator it = l.begin(); it != l.end(); ++it ) {
        lst.append( new QRegExp(*it,cs,true) );
    }
  }

  return lst;
}

bool HiddenFileView::matchHidden(const QString & s)
{
  QPtrList<QRegExp> hiddenList(_hiddenList);

  if (_dlg->hideDotFilesChk->isChecked())
     hiddenList.append( new QRegExp(".*",false,true) );

  return matchRegExpList(s,hiddenList);
}

bool HiddenFileView::matchVeto(const QString & s)
{
  return matchRegExpList(s,_vetoList);
}

bool HiddenFileView::matchRegExpList(const QString & s, QPtrList<QRegExp> & lst)
{
  if (getRegExpListMatch(s,lst))
     return true;
  else
     return false;
}


QRegExp* HiddenFileView::getHiddenMatch(const QString & s)
{
  return getRegExpListMatch(s,_hiddenList);
}

QRegExp* HiddenFileView::getVetoMatch(const QString & s)
{
  return getRegExpListMatch(s,_vetoList);
}

QRegExp* HiddenFileView::getRegExpListMatch(const QString & s, QPtrList<QRegExp> & lst)
{
  QRegExp* rx;

  for ( rx = lst.first(); rx; rx = lst.next() )
  {
    if (rx->exactMatch(s))
       return rx;
  }

  return 0L;
}

void HiddenFileView::hideDotFilesChkClicked(bool b)
{
  updateView();
}

void HiddenFileView::hideUnreadableChkClicked(bool b)
{
  updateView();
}

#include "hiddenfileview.moc"
