/***************************************************************************
                          nfshostdlg.h  -  description
                             -------------------
    begin                : Mon Apr 29 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NFSHOSTDLG_H
#define NFSHOSTDLG_H

#include <kdialogbase.h>


/**
  *@author Jan Schäfer
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
