/***************************************************************************
                          sharedlgimpl.h  -  description
                             -------------------
    begin                : Tue June 6 2002
    copyright            : (C) 2002 by Jan Schäfer
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

#ifndef SHAREDLGIMPL_H
#define SHAREDLGIMPL_H


/**
 * @author Jan Schäfer
 **/

#include <qptrlist.h>

#include <kfileitem.h>
#include <klistview.h>

#include "share.h"

class SambaShare;
class QWidget;
class KDirLister;
class QRegExp;
class KPopupMenu;
class KToggleAction;
class QGroupBox;

class HiddenListViewItem : public KListViewItem
{
public:
  HiddenListViewItem( QListView *parent, KFileItem *fi, bool hidden, bool veto );
  ~HiddenListViewItem();

  void setVeto(bool b);
  bool isVeto();

  void setHidden(bool b);
  bool isHidden();

  virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment);
protected:
  KFileItem *_fileItem;
  bool _hidden;
  bool _veto;
};



/**
 * Widget which shows a list of files
 * from a directory.
 * It gets the directory from the SambaShare
 * It also interprets the hidden files parameter
 * of the SambaShare an offers the possibility of
 * selecting the files which should be hidden
 **/
class HiddenFileView : public QWidget
{
Q_OBJECT

public:
  HiddenFileView(QWidget* parent, SambaShare* share);
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
  KListView* _listView;
  KDirLister* _dir;
  QPtrList<QRegExp> _hiddenList;
  QPtrList<QRegExp> _vetoList;

  QLineEdit* _hiddenEdit;
  QLineEdit* _vetoEdit;

  QCheckBox* _hiddenChk;
  QCheckBox* _vetoChk;

  KToggleAction* _hiddenActn;
  KToggleAction* _vetoActn;

  QGroupBox* _selGrpBx;

  KPopupMenu* _popup;

  void initListView();

  QPtrList<QRegExp> createRegExpList(const QString & s);
  bool matchHidden(const QString & s);
  bool matchVeto(const QString & s);
  bool matchRegExpList(const QString & s, QPtrList<QRegExp> & lst);

  QRegExp* getHiddenMatch(const QString & s);
  QRegExp* getVetoMatch(const QString & s);
  QRegExp* getRegExpListMatch(const QString & s, QPtrList<QRegExp> & lst);

  QPtrList<HiddenListViewItem> getMatchingItems(const QRegExp & rx);

  void setVeto(QPtrList<HiddenListViewItem> & lst, bool b);
  void setHidden(QPtrList<HiddenListViewItem> & lst, bool b);
  void deselect(QPtrList<HiddenListViewItem> & lst);

  void updateEdit(QLineEdit* edit, QPtrList<QRegExp> & lst);

protected slots:
  void insertNewFiles(const KFileItemList &newone);
  void selectionChanged();
  void hiddenChkClicked(bool b);
  void vetoChkClicked(bool b);
  void showContextMenu(QListViewItem*,const QPoint&);
  void updateView();
};

/**
 * This class implements the share.ui interface
 **/
class ShareDlgImpl : public KcmShareDlg
{
Q_OBJECT

public :

  ShareDlgImpl(QWidget* parent, SambaShare* share);
  ~ShareDlgImpl();

protected :

	/**
   * Fills all dialog fields with the values
   * of the SambaShare object
   **/
  void initDialog();
  
  /**
   * The share object to change with this dialog
   **/
	SambaShare* _share;

  QWidget* _hiddenFilesTab;
  HiddenFileView* _fileView;

  void createHiddenFilesTab();
  void createHiddenFilesView();


protected slots:
	virtual void accept();
  virtual void homeChkToggled(bool);
protected slots: // Protected slots
  /** No descriptions */
  void tabChangedSlot(QWidget* w);
};


#endif
