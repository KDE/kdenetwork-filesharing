/***************************************************************************
                          dictmanager.cpp  -  description
                             -------------------
    begin                : Wed Jan 1 2003
    copyright            : (C) 2003 by Jan Schäfer
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/
 
#include <iostream.h> 
 
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>

#include <kurlrequester.h> 
 
#include "sambashare.h"
#include "dictmanager.h" 

 
DictManager::DictManager():
  lineEditDict(40,false),
  checkBoxDict(40,false),
  urlRequesterDict(40,false),
  spinBoxDict(40,false),
  comboBoxDict(20,false)
{
}

DictManager::~DictManager() {
}

void DictManager::add(const QString & key, QLineEdit* lineEdit) {
  lineEditDict.insert(key,lineEdit);
  connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(changedSlot()));
}

void DictManager::add(const QString & key, QCheckBox* checkBox){
  checkBoxDict.insert(key,checkBox);
  connect(checkBox, SIGNAL(clicked()), this, SLOT(changedSlot()));
}

void DictManager::add(const QString & key, KURLRequester* urlRq){
  urlRequesterDict.insert(key,urlRq);
  connect(urlRq, SIGNAL(textChanged(const QString &)), this, SLOT(changedSlot()));
}

void DictManager::add(const QString & key, QSpinBox* spinBox){
  spinBoxDict.insert(key,spinBox);
  connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(changedSlot()));
}
              
void DictManager::add(const QString & key, QComboBox* comboBox){
  comboBoxDict.insert(key,comboBox);
  connect(comboBox, SIGNAL(activated(int)), this, SLOT(changedSlot()));
}


void DictManager::load(SambaShare* share, bool globalValue, bool defaultValue){
  QDictIterator<QCheckBox> checkBoxIt( checkBoxDict ); 
   
  for( ; checkBoxIt.current(); ++checkBoxIt )	{
    checkBoxIt.current()->setChecked(share->getBoolValue(checkBoxIt.currentKey(),globalValue,defaultValue));
  }

  QDictIterator<QLineEdit> lineEditIt( lineEditDict ); 
 
  for( ; lineEditIt.current(); ++lineEditIt )	{
    lineEditIt.current()->setText(share->getValue(lineEditIt.currentKey(),globalValue,defaultValue));
  }

  QDictIterator<KURLRequester> urlRequesterIt( urlRequesterDict ); 
 
  for( ; urlRequesterIt.current(); ++urlRequesterIt )	{
    urlRequesterIt.current()->setURL(share->getValue(urlRequesterIt.currentKey(),globalValue,defaultValue));
  }

  QDictIterator<QSpinBox> spinBoxIt( spinBoxDict ); 
   
  for( ; spinBoxIt.current(); ++spinBoxIt )	{
    spinBoxIt.current()->setValue(share->getValue(spinBoxIt.currentKey(),globalValue,defaultValue).toInt());
  }

  QDictIterator<QComboBox> comboBoxIt( comboBoxDict ); 
   
  for( ; comboBoxIt.current(); ++comboBoxIt )	{
    comboBoxIt.current()->setCurrentText( share->getValue(comboBoxIt.currentKey(),globalValue,defaultValue) );
  }

}

void DictManager::save(SambaShare* share, bool globalValue, bool defaultValue){
  QDictIterator<QCheckBox> checkBoxIt( checkBoxDict ); 
 
  for( ; checkBoxIt.current(); ++checkBoxIt )	{
    share->setValue(checkBoxIt.currentKey(),checkBoxIt.current()->isChecked(), globalValue, defaultValue );
  }

  QDictIterator<QLineEdit> lineEditIt( lineEditDict ); 
   
  for( ; lineEditIt.current(); ++lineEditIt )	{
    share->setValue(lineEditIt.currentKey(),lineEditIt.current()->text(), globalValue, defaultValue );
  }

  QDictIterator<KURLRequester> urlRequesterIt( urlRequesterDict ); 
 
  for( ; urlRequesterIt.current(); ++urlRequesterIt )	{
    share->setValue(urlRequesterIt.currentKey(),urlRequesterIt.current()->url(), globalValue, defaultValue );
  }

  QDictIterator<QSpinBox> spinBoxIt( spinBoxDict ); 
 
  for( ; spinBoxIt.current(); ++spinBoxIt )	{
    share->setValue(spinBoxIt.currentKey(),spinBoxIt.current()->value(), globalValue, defaultValue );
  }

  QDictIterator<QComboBox> comboBoxIt( comboBoxDict );
  
  for( ; comboBoxIt.current(); ++comboBoxIt )	{
    share->setValue(comboBoxIt.currentKey(),comboBoxIt.current()->currentText(), globalValue, defaultValue );
  }

}

void DictManager::changedSlot() {
  emit changed();
}



#include "dictmanager.moc"


