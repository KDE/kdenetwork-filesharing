/***************************************************************************
                          sharedlgimpl.h  -  description
                             -------------------
    begin                : Tue June 6 2002
    copyright            : (C) 2002 by Jan Sch�fer
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
 *  along with Foobar; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#ifndef SHAREDLGIMPL_H
#define SHAREDLGIMPL_H


/**
 * @author Jan Sch�fer
 **/

#include "share.h"

class SambaShare;
class QWidget;

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

protected slots:
	virtual void accept();
};


#endif
