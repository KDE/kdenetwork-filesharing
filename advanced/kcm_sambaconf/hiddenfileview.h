/***************************************************************************
                          hiddenfileview.h  -  description
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

#ifndef _HIDDENFILEVIEW_H_
#define _HIDDENFILEVIEW_H_

#include <kfileitem.h>
#include <klistview.h>

#include "qmultichecklistitem.h"

class KDirLister;
class QRegExp;
class ShareDlgImpl;
class SambaShare;

class HiddenListViewItem : public QMultiCheckListItem
{
Q_OBJECT
public:
  HiddenListViewItem( QListView *parent, KFileItem *fi, bool hidden, bool veto, bool vetoOplock );
  ~HiddenListViewItem();

  virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment);

  KFileItem* getFileItem();
protected:
  KFileItem *_fileItem;
};

class KToggleAction;
class KPopupMenu;
class ShareDlgImpl;


/**
 * Widget which shows a list of files
 * from a directory.
 * It gets the directory from the SambaShare
 * It also interprets the hidden files parameter
 * of the SambaShare an offers the possibility of
 * selecting the files which should be hidden
 **/
class HiddenFileView : public QObject
{
Q_OBJECT

public:
  HiddenFileView(ShareDlgImpl* shareDlg, SambaShare* share);
  ~HiddenFileView();

  /**
   * Load the values from the share and show them
   **/
  void load();

  /**
   * Save changings to the share
   **/
  void save();

protected:
  SambaShare* _share;
  ShareDlgImpl* _dlg;

  KDirLister* _dir;
  QPtrList<QRegExp> _hiddenList;
  QPtrList<QRegExp> _vetoList;
  QPtrList<QRegExp> _vetoOplockList;

  KToggleAction* _hiddenActn;
  KToggleAction* _vetoActn;
  KToggleAction* _vetoOplockActn;

  KPopupMenu* _popup;

  void initListView();

  QPtrList<QRegExp> createRegExpList(const QString & s);
  bool matchHidden(const QString & s);
  bool matchVeto(const QString & s);
  bool matchVetoOplock(const QString & s);
  bool matchRegExpList(const QString & s, QPtrList<QRegExp> & lst);

  QRegExp* getHiddenMatch(const QString & s);
  QRegExp* getVetoMatch(const QString & s);
  QRegExp* getRegExpListMatch(const QString & s, QPtrList<QRegExp> & lst);

  QPtrList<HiddenListViewItem> getMatchingItems(const QRegExp & rx);

  void setState(QPtrList<HiddenListViewItem> & lst,int column, bool b);
  void deselect(QPtrList<HiddenListViewItem> & lst);

  void updateEdit(QLineEdit* edit, QPtrList<QRegExp> & lst);

protected slots:
  // slots for KDirListener :
  void insertNewFiles(const KFileItemList &newone);
  void deleteItem( KFileItem *_fileItem );
  void refreshItems( const KFileItemList& items );
  
  void selectionChanged();
  void hiddenChkClicked(bool b);
  void vetoChkClicked(bool b);
  void vetoOplockChkClicked(bool b);
  void checkBoxClicked(QCheckBox* chkBox,KToggleAction* action,QLineEdit* edit,int column,QPtrList<QRegExp> &reqExpList,bool b);
  void columnClicked(int column);
  void showContextMenu();
  void updateView();
  void hideDotFilesChkClicked(bool);
  void hideUnreadableChkClicked(bool);
  void slotMouseButtonPressed( int button, QListViewItem * item, const QPoint & pos, int c );
};

#endif
