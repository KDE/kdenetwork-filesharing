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

#include <assert.h>

#include <stdio.h>
 
#include <qlineedit.h>
#include <qcheckbox.h>

#include "filemodedlgimpl.h"

FileModeDlgImpl::FileModeDlgImpl(QWidget* parent, QLineEdit* edit)
  : FileModeDlg(parent)
{
  assert(edit);
  _edit = edit;

  init();
}

FileModeDlgImpl::~FileModeDlgImpl()
{
}

void FileModeDlgImpl::init()
{
  bool ok;
  int mod = _edit->text().toInt(&ok,8);

  if (!ok)
     mod = 0;
  
  stickyBitChk->setChecked( mod & 01000 );
  setGIDChk->setChecked( mod & 02000 );
  setUIDChk->setChecked( mod & 04000 );

  ownerExecChk->setChecked( mod & 0100 );
  ownerWriteChk->setChecked( mod & 0200 );
  ownerReadChk->setChecked( mod & 0400 );
  
  groupExecChk->setChecked( mod & 010 );
  groupWriteChk->setChecked( mod & 020 );
  groupReadChk->setChecked( mod & 040 );
  
  othersExecChk->setChecked( mod & 01 );
  othersWriteChk->setChecked( mod & 02 );
  othersReadChk->setChecked( mod & 04 );
  
}

void FileModeDlgImpl::accept()
{
  QString s = ""; 

  s += QString::number(
        (stickyBitChk->isChecked() ? 1 : 0) +
        (setGIDChk->isChecked() ? 2 : 0) +
        (setUIDChk->isChecked() ? 4 : 0)
       );
       
  s += QString::number(
        (ownerExecChk->isChecked() ? 1 : 0) +
        (ownerWriteChk->isChecked() ? 2 : 0) +
        (ownerReadChk->isChecked() ? 4 : 0)
       );

  s += QString::number(
        (groupExecChk->isChecked() ? 1 : 0) +
        (groupWriteChk->isChecked() ? 2 : 0) +
        (groupReadChk->isChecked() ? 4 : 0)
       );

  s += QString::number(
        (othersExecChk->isChecked() ? 1 : 0) +
        (othersWriteChk->isChecked() ? 2 : 0) +
        (othersReadChk->isChecked() ? 4 : 0)
       );

  // it's an octal number so start with a 0
  // but remove all the other trailing 0's
  s = QString::number( s.toInt());
  s = "0"+s;

  _edit->setText(s);

  

  FileModeDlg::accept();
}

#include "filemodedlgimpl.moc"

