/*
  Copyright (c) 2002 Laurent Montel <lmontel@mandrakesoft.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef __fileshare_h__
#define __fileshare_h__

#include <kfileitem.h>

#include "kcmodule.h"
class QRadioButton;
class QLabel;
class QBoxLayout;
class ControlCenterGUI;
class QListViewItem;

class KFileShareConfig  : public KCModule
{
  Q_OBJECT

 public:
  KFileShareConfig(QWidget *parent, const char *name, const QStringList &);

  virtual void load();
  virtual void save();
  virtual void defaults();
  virtual QString quickHelp() const;

 protected:  
   ControlCenterGUI* m_ccgui;
   QString m_fileShareGroup;
   bool m_restricted;
   bool m_rootPassNeeded;
   QString m_smbConf;
   void showShareDialog(const KFileItemList & files);
   bool addGroupAccessesToFile(const QString & file);
   bool removeGroupAccessesFromFile(const QString & file);
   bool setGroupAccesses();
   
 protected slots:
  void configChanged() { emit changed( true ); };
  void updateShareListView(); 
  void allowedUsersBtnClicked();
  
  virtual void addShareBtnClicked();
  virtual void changeShareBtnClicked();
  virtual void removeShareBtnClicked();
  
};

class PropertiesPageDlg : public KDialogBase
{
Q_OBJECT
public: 
  PropertiesPageDlg(QWidget * parent, KFileItemList files);
  ~PropertiesPageDlg() {};
  bool hasChanged();
protected:
  PropertiesPage* m_page;
  
protected slots:
  virtual void slotOk();

};



#endif
