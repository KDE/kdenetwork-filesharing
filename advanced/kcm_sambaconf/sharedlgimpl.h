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

#include "share.h"

class SambaShare;
class QWidget;
class KPopupMenu;
class KToggleAction;
class QGroupBox;
class UserTabImpl;
class HiddenFileView;
class DictManager;
class KJanusWidget;

/**
 * This class implements the share.ui interface
 **/
class ShareDlgImpl : public KcmShareDlg
{
Q_OBJECT

public :

  ShareDlgImpl(QWidget* parent, SambaShare* share);
  ~ShareDlgImpl();

  bool hasChanged() { return m_changed; };
  
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

  HiddenFileView* _fileView;
  UserTabImpl* _userTab;
  KJanusWidget* _janus;
  bool m_changed;
  DictManager* _dictMngr;

  void loadHiddenFilesView();
  void initAdvancedTab();

protected slots:
	virtual void accept();
  virtual void homeChkToggled(bool);
  virtual void accessModifierBtnClicked();
  virtual void changedSlot();
  virtual void pathUrlRq_textChanged( const QString & );

  void tabChangedSlot(QWidget* w);
signals:
  void changed();
};


#endif
