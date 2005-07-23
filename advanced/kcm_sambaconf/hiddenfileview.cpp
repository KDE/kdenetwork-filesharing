/***************************************************************************
                          hiddenfileview.cpp  -  description
                             -------------------
    begin                : Wed Jan 1 2003
    copyright            : (C) 2003 by Jan Sch�er
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
#define COL_VETO_OPLOCK 3
#define COL_SIZE 4
#define COL_DATE 5
#define COL_PERM 6
#define COL_OWNER 7
#define COL_GROUP 8

#define HIDDENTABINDEX 5

HiddenListViewItem::HiddenListViewItem( QListView *parent, KFileItem *fi, bool hidden=false, bool veto=false, bool vetoOplock=false )
  : QMultiCheckListItem( parent )
{
  setPixmap( COL_NAME, fi->pixmap(KIcon::SizeSmall));

  setText( COL_NAME, fi->text() );
  setText( COL_SIZE, KGlobal::locale()->formatNumber( fi->size(), 0));
  setText( COL_DATE,  fi->timeString() );
  setText( COL_PERM,  fi->permissionsString() );
  setText( COL_OWNER, fi->user() );
  setText( COL_GROUP, fi->group() );

  setOn(COL_HIDDEN,hidden);
  setOn(COL_VETO,veto);
  setOn(COL_VETO_OPLOCK,vetoOplock);

  _fileItem = fi;
}

HiddenListViewItem::~HiddenListViewItem()
{
}

KFileItem* HiddenListViewItem::getFileItem()
{
  return _fileItem;
}


void HiddenListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
  QColorGroup _cg = cg;

  if (isOn(COL_VETO))
     _cg.setColor(QColorGroup::Base,lightGray);

  if (isOn(COL_HIDDEN))
     _cg.setColor(QColorGroup::Text,gray);

  QMultiCheckListItem::paintCell(p, _cg, column, width, alignment);
}




HiddenFileView::HiddenFileView(ShareDlgImpl* shareDlg, SambaShare* share)
{
  _share = share;
  _dlg = shareDlg;

  _hiddenActn = new KToggleAction(i18n("&Hide"));
  _vetoActn = new KToggleAction(i18n("&Veto"));
  _vetoOplockActn = new KToggleAction(i18n("&Veto Oplock"));

  initListView();

  _dlg->hiddenChk->setTristate(true);
  _dlg->vetoChk->setTristate(true);

  connect( _dlg->hiddenChk, SIGNAL(toggled(bool)), this, SLOT( hiddenChkClicked(bool) ));
  connect( _dlg->vetoChk, SIGNAL(toggled(bool)), this, SLOT( vetoChkClicked(bool) ));
  connect( _dlg->vetoOplockChk, SIGNAL(toggled(bool)), this, SLOT( vetoOplockChkClicked(bool) ));

  _dlg->hiddenEdit->setText( _share->getValue("hide files") );
  connect( _dlg->hiddenEdit, SIGNAL(textChanged(const QString &)), this, SLOT(updateView()));

  _dlg->vetoEdit->setText( _share->getValue("veto files") );
  connect( _dlg->vetoEdit, SIGNAL(textChanged(const QString &)), this, SLOT(updateView()));

  _dlg->vetoOplockEdit->setText( _share->getValue("veto oplock files") );
  connect( _dlg->vetoOplockEdit, SIGNAL(textChanged(const QString &)), this, SLOT(updateView()));

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
  connect( _vetoOplockActn, SIGNAL(toggled(bool)), this, SLOT(vetoOplockChkClicked(bool)));
}

void HiddenFileView::initListView()
{
  _dlg->hiddenListView->setMultiSelection(true);
  _dlg->hiddenListView->setSelectionMode(QListView::Extended);
  _dlg->hiddenListView->setAllColumnsShowFocus(true);

  _hiddenList = createRegExpList(_share->getValue("hide files"));
  _vetoList = createRegExpList(_share->getValue("veto files"));
  _vetoOplockList = createRegExpList(_share->getValue("veto oplock files"));

  _popup = new KPopupMenu(_dlg->hiddenListView);

  _hiddenActn->plug(_popup);
  _vetoActn->plug(_popup);
  _vetoOplockActn->plug(_popup);

  connect( _dlg->hiddenListView, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  connect( _dlg->hiddenListView, SIGNAL(contextMenu(KListView*,QListViewItem*,const QPoint&)),
           this, SLOT(showContextMenu()));

  connect( _dlg->hideDotFilesChk, SIGNAL(toggled(bool)), this, SLOT(hideDotFilesChkClicked(bool)));
  connect( _dlg->hideUnreadableChk, SIGNAL(toggled(bool)), this, SLOT(hideUnreadableChkClicked(bool)));

  connect( _dlg->hiddenListView, SIGNAL(mouseButtonPressed(int,QListViewItem*,const QPoint &,int)),
           this, SLOT(slotMouseButtonPressed(int,QListViewItem*,const QPoint &,int)));
}

HiddenFileView::~HiddenFileView()
{
    delete _dir;
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
  if ( (!s.isEmpty()) && (s.right(1)!="/"))
      s+="/";
  _share->setValue("hide files", s);

  s = _dlg->vetoEdit->text().stripWhiteSpace();
  // its important that the string ends with an '/'
  // otherwise Samba won't recognize the last entry
  if ( (!s.isEmpty()) && (s.right(1)!="/"))
      s+="/";
  _share->setValue("veto files", s);

  s = _dlg->vetoOplockEdit->text().stripWhiteSpace();
  // its important that the string ends with an '/'
  // otherwise Samba won't recognize the last entry
  if ( (!s.isEmpty()) && (s.right(1)!="/"))
      s+="/";
  _share->setValue("veto oplock files", s);

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
    bool vetoOplock = matchVetoOplock(tmp->text());

    new HiddenListViewItem( _dlg->hiddenListView, tmp, hidden, veto, vetoOplock );

  }
}



void HiddenFileView::columnClicked(int column) {


  switch (column) {
    case COL_HIDDEN : hiddenChkClicked( !_dlg->hiddenChk->isOn() );break;
    case COL_VETO : vetoChkClicked( !_dlg->vetoChk->isOn() );break;
    case COL_VETO_OPLOCK : vetoOplockChkClicked( !_dlg->vetoOplockChk->isOn() );break;
    default : break;
  }
}

void HiddenFileView::deleteItem( KFileItem *fileItem )
{
  HiddenListViewItem* item;
  for (item = dynamic_cast<HiddenListViewItem*>(_dlg->hiddenListView->firstChild());item;
       item = dynamic_cast<HiddenListViewItem*>(item->nextSibling()))
  {
    if (item->getFileItem() == fileItem)
    {
      delete item;
      break;
    }
  }

}

void HiddenFileView::refreshItems( const KFileItemList& /*items*/ )
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
  bool vetoOplock = false;
  bool noVetoOplock = false;

  int n = 0;

  HiddenListViewItem* item;
  for (item = static_cast<HiddenListViewItem*>(_dlg->hiddenListView->firstChild());item;
       item = static_cast<HiddenListViewItem*>(item->nextSibling()))
  {
    if (!item->isSelected())
       continue;

    n++;

    if (item->isOn(COL_VETO))
       veto = true;
    else
       noVeto = true;

    if (item->isOn(COL_VETO_OPLOCK))
       vetoOplock = true;
    else
       noVetoOplock = true;

    if (item->isOn(COL_HIDDEN))
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

  if (vetoOplock && noVetoOplock)
  {
    _dlg->vetoOplockChk->setTristate(true);
    _dlg->vetoOplockChk->setNoChange();
    _dlg->vetoOplockChk->update();
  }
  else
  {
    _dlg->vetoOplockChk->setTristate(false);
    _dlg->vetoOplockChk->setChecked(vetoOplock);
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

void HiddenFileView::checkBoxClicked(QCheckBox* chkBox,KToggleAction* action,QLineEdit* edit, int column,QPtrList<QRegExp> & reqExpList,bool b) {
  // We don't save the old state so
  // disable the tristate mode
  chkBox->setTristate(false);
  action->setChecked(b);
  chkBox->setChecked(b);

  HiddenListViewItem* item;
  for (item = static_cast<HiddenListViewItem*>(_dlg->hiddenListView->firstChild());item;
       item = static_cast<HiddenListViewItem*>(item->nextSibling()))
  {
    if (!item->isSelected())
        continue;
        
    if (b == item->isOn(column))
        continue;
            
    if (!b) {
        QRegExp* rx = getRegExpListMatch(item->text(0),reqExpList);
        
        // Perhaps the file was hidden because it started with a dot
        if (!rx && item->text(0)[0]=='.' && _dlg->hideDotFilesChk->isChecked()) {
            int result = KMessageBox::questionYesNo(_dlg,i18n(
                    "<qt>Some files you have selected are hidden because they start with a dot; "
                    "do you want to uncheck all files starting with a dot?</qt>"),i18n("Files Starting With Dot"),i18n("Uncheck Hidden"), i18n("Keep Hidden"));
                
            if (result == KMessageBox::No) {
                QPtrList<HiddenListViewItem> lst = getMatchingItems(QRegExp(".*",false,true));
                deselect(lst);
            } else {
                _dlg->hideDotFilesChk->setChecked(false);
            }
            continue;
        } else {
            if (rx) {
                // perhaps it is matched by a wildcard string
                QString p = rx->pattern();
                if ( p.find("*") > -1 ||
                        p.find("?") > -1 )
                {
                    // TODO after message freeze: why show three times the wildcard string? Once should be enough.
		    // TODO remove <b></b> and use <qt> instead
                    int result = KMessageBox::questionYesNo(_dlg,i18n(
                    "<b></b>Some files you have selected are matched by the wildcarded string <b>'%1'</b>; "
                    "do you want to uncheck all files matching <b>'%1'</b>?").arg(rx->pattern()).arg(rx->pattern()).arg(rx->pattern()),
                    i18n("Wildcarded String"),i18n("Uncheck Matches"),i18n("Keep Selected"));
            
                    QPtrList<HiddenListViewItem> lst = getMatchingItems( *rx );
            
                    if (result == KMessageBox::No) {
                        deselect(lst);
                    } else {
                        setState(lst,column,false);
                        reqExpList.remove(rx);
                        updateEdit(edit, reqExpList);
                    }
                    continue;
                } else {
                    reqExpList.remove(rx);
                    updateEdit(edit, reqExpList);
                }
            }   
        }
    }
    else {
        reqExpList.append( new QRegExp(item->text(0)) );
        updateEdit(edit, reqExpList);
    }
    
    item->setOn(column,b);
  }

  _dlg->hiddenListView->update();
}

void HiddenFileView::hiddenChkClicked(bool b)
{
    checkBoxClicked(_dlg->hiddenChk, _hiddenActn, _dlg->hiddenEdit, COL_HIDDEN,_hiddenList,b);

}

void HiddenFileView::vetoOplockChkClicked(bool b) {
    checkBoxClicked(_dlg->vetoOplockChk, _vetoOplockActn, _dlg->vetoOplockEdit, COL_VETO_OPLOCK,_vetoOplockList,b);
}

void HiddenFileView::vetoChkClicked(bool b)
{
    checkBoxClicked(_dlg->vetoChk, _vetoActn, _dlg->vetoEdit, COL_VETO,_vetoList,b);
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


void HiddenFileView::setState(QPtrList<HiddenListViewItem> & lst, int column, bool b) {
  HiddenListViewItem* item;
  for(item = static_cast<HiddenListViewItem*>(lst.first()); item;
      item = static_cast<HiddenListViewItem*>(lst.next()) )
  {
    item->setOn(column,b);
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
  _vetoOplockList = createRegExpList(_dlg->vetoOplockEdit->text());

  HiddenListViewItem* item;
  for (item = static_cast<HiddenListViewItem*>(_dlg->hiddenListView->firstChild());item;
       item = static_cast<HiddenListViewItem*>(item->nextSibling()))
  {
    item->setOn(COL_HIDDEN,matchHidden(item->text(0)));
    item->setOn(COL_VETO,matchVeto(item->text(0)));
    item->setOn(COL_VETO_OPLOCK,matchVetoOplock(item->text(0)));
  }

  _dlg->hiddenListView->repaint();
}


QPtrList<QRegExp> HiddenFileView::createRegExpList(const QString & s)
{
  QPtrList<QRegExp> lst;
  bool cs = _share->getBoolValue("case sensitive");

  if (!s.isEmpty())
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

bool HiddenFileView::matchVetoOplock(const QString & s)
{
  return matchRegExpList(s,_vetoOplockList);
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

void HiddenFileView::hideDotFilesChkClicked(bool)
{
  updateView();
}

void HiddenFileView::hideUnreadableChkClicked(bool)
{
  updateView();
}

void HiddenFileView::slotMouseButtonPressed( int, QListViewItem*, const QPoint&, int c ) {
  columnClicked(c);
}


#include "hiddenfileview.moc"
