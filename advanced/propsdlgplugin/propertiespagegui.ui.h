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


bool PropertiesPageGUI::hasChanged()
{
    return m_hasChanged;
}


void PropertiesPageGUI::sambaChkToggled( bool )
{

}


void PropertiesPageGUI::publicSambaChkToggled( bool b)
{
    if (!b) {
	writableSambaChk->setChecked(false);
    }
}


void PropertiesPageGUI::publicNFSChkToggled( bool b)
{
    if (!b) {
	writableNFSChk->setChecked(false);
    }

}


void PropertiesPageGUI::moreSambaBtnClicked()
{

}
