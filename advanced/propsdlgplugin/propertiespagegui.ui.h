/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void PropertiesPageGUI::changedSlot()
{
    m_hasChanged = true;
    emit changed();
}


void PropertiesPageGUI::moreNFSBtn_clicked()
{

}


void PropertiesPageGUI::urlRqTextChanged( const QString & )
{

}


bool PropertiesPageGUI::hasChanged()
{
    return m_hasChanged;
}
