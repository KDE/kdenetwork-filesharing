/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>

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

#ifndef PROPERTIESPAGE_H
#define PROPERTIESPAGE_H

#include <kfileitem.h>
#include "propertiespagegui.h"

class NFSFile;
class NFSEntry;
class SambaFile;
class SambaShare;
class QCheckBox;

class PropertiesPage : public PropertiesPageGUI
{
Q_OBJECT
public:
  PropertiesPage(QWidget* parent, KFileItemList items, bool enterUrl=false);
  virtual ~PropertiesPage();
  
  bool save();

  static bool save(NFSFile* nfsFile, SambaFile* sambFile, bool nfs, bool samba); 
public slots:
  void load();
    
protected:
  
  bool m_enterUrl;
  QString m_path;
  KFileItemList m_items;  
  NFSFile *m_nfsFile;
  NFSEntry *m_nfsEntry;
  
  SambaFile *m_sambaFile;
  SambaShare *m_sambaShare;
  bool m_sambaChanged;
  bool m_nfsChanged;
  bool m_loaded;
  
protected slots:
  // inherited from PropertiesPageGUI
  virtual void moreNFSBtn_clicked();
  virtual void moreSambaBtnClicked();
  virtual void sambaChkToggled( bool b );
  virtual void urlRqTextChanged(const QString&);
  
private:
  bool loadNFS();  
  void loadNFSEntry();  
  void updateNFSEntry();
  bool saveNFS();
  
  bool loadSamba();
  void loadSambaShare();
  bool updateSambaShare();
  bool saveSamba();
  
  bool checkURL();
  void setSambaShareBoolValue(const QString & value, QCheckBox* chk);
  void createNewSambaShare();
  QString getNewSambaName();
  
  void enableNFS(bool b,const QString & message);
  void enableSamba(bool b,const QString & message);
  
};

#endif
