/***************************************************************************
                          filemodedlgimpl.cpp  -  description
                             -------------------
    begin                : Thu Jul 18 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KSambaPlugin.                                        *
 *                                                                            *
 *  KSambaPlugin is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KSambaPlugin is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with KSambaPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#ifndef FILEMODEDLGIMPL_H
#define FILEMODEDLGIMPL_H

/**
  *@author Jan Schäfer
  */

#include "filemodedlg.h"

class QLineEdit;

/**
 * Small dialog to change the UNIX access rights
 * It is called with a QLineEdit as parameter.
 * The class then takes the text from the QLineEdit and
 * interprets it as an oktal UNIX access right string
 * e.g. 0744
 * After the user has changed the access rights with the dialog
 * the class sets the new access rights as a new octal string
 * of the QLineEdit
 * Implements the filemodedlg.ui interface
 **/  
class FileModeDlgImpl : public FileModeDlg
{
Q_OBJECT
public: 
	FileModeDlgImpl(QWidget* parent, QLineEdit* edit);
	~FileModeDlgImpl();
protected:
  QLineEdit* _edit;
  void init();
protected slots:
  virtual void accept();  
};

#endif
