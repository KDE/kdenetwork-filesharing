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

class PropertiesPage : public PropertiesPageGUI
{
Q_OBJECT
public:
  PropertiesPage(QWidget* parent, KFileItemList items, bool enterUrl=false);
  virtual ~PropertiesPage();
  
  bool save();
  
protected:
  bool load();
  
  QString m_path;
  KFileItemList m_items;  
  NFSFile *m_nfsFile;
  NFSEntry *m_nfsEntry;
  bool m_enterUrl;
  
protected slots:
  // inherited from PropertiesPageGUI
  virtual void moreNFSBtn_clicked();
  virtual void urlRqTextChanged( const QString & );
private:
  bool loadNFS();  
  void loadNFSEntry();  
  bool updateNFSEntry();
  bool saveNFS();
  
  void disableNFS(const QString & message);
  void disableSamba(const QString & message);
  
};

#endif
