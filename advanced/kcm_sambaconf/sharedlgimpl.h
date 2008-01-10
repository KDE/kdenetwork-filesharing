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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA  *
 *                                                                            *
 ******************************************************************************/

#ifndef SHAREDLGIMPL_H
#define SHAREDLGIMPL_H


/**
 * @author Jan Schäfer
 **/

#include <q3ptrlist.h>
#include <QDialog>
#include <K3Process>

#include <ui_share.h>

class SambaShare;
class QWidget;
class UserTabImpl;
class HiddenFileView;
class DictManager;
class KPageWidget;

/**
 * This class implements the share.ui interface
 **/
class ShareDlgImpl : public QDialog, public Ui::KcmShareDlg
{
Q_OBJECT

public :

  ShareDlgImpl(QWidget* parent, SambaShare* share);
  ~ShareDlgImpl();

  bool hasChanged() { return m_changed; }
  
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
  KPageWidget* _janus;
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
  void buttonHelp_clicked();
  void oplocksChk_toggled( bool b);
  void tabChangedSlot(QWidget* w);
  void publicBaseChk_toggled( bool b);
  void lockingChk_toggled( bool b);
  void oplockContentionLimitSpin_valueChanged( int i);
  void fakeOplocksChk_toggled( bool b);
  void storeDosAttributesChk_toggled( bool b);
  void checkValues();
  void userOnlyChk_toggled( bool b);
  void guestOnlyChk_toggled( bool b);

signals:
  void changed();
};


#endif
