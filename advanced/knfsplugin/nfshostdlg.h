/***************************************************************************
                          nfshostdlg.h  -  description
                             -------------------
    begin                : Mon Apr 29 2002
    copyright            : (C) 2002 by Jan Sch�fer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KNfsPlugin.                                          *
 *                                                                            *
 *  KNfsPlugin is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KNfsPlugin is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with KNfsPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#ifndef NFSHOSTDLG_H
#define NFSHOSTDLG_H

#include <kdialogbase.h>


/**
  *@author Jan Sch�fer
  */
class NFSHost;
class HostProps;


class NFSHostDlg : public KDialogBase
{
Q_OBJECT
public: 
	NFSHostDlg(QWidget* parent, NFSHost* host);
	virtual ~NFSHostDlg();
protected:
  NFSHost* _host;
  HostProps* _hostProps;

  void init(NFSHost*);
protected slots:
  void slotOk();
  void slotCancel();
  void setModified();
signals :
  void modified();
};

#endif
