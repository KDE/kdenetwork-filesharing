/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

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

void KcmInterface::init()
{
    addShareBtn->setIconSet(SmallIcon("filenew"));
    editShareBtn->setIconSet(SmallIcon("edit"));
    removeShareBtn->setIconSet(SmallIcon("editdelete"));
    editDefaultShareBtn->setIconSet(SmallIcon("queue"));
    
    addPrinterBtn->setIconSet(SmallIcon("filenew"));
    editPrinterBtn->setIconSet(SmallIcon("edit"));
    removePrinterBtn->setIconSet(SmallIcon("editdelete"));
    editDefaultPrinterBtn->setIconSet(SmallIcon("print_class"));
    
}



void KcmInterface::changedSlot()
{
    emit changed();
}


