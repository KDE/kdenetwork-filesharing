/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


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

