/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/
#include "nfsfile.h"


void NFSDialog::addHostClicked()
{

}

void NFSDialog::removeHostClicked()
{

}

void NFSDialog::shareStateChanged( int state )
{
    if (state==0)
       groupBox->setEnabled(false);
    else
       groupBox->setEnabled(true);
	
}



void NFSDialog::specifyPathSlot()
{
  
}