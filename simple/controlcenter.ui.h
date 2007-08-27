/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#ifndef SIMPLE_CONTROLCENTER_UI_H
#define SIMPLE_CONTROLCENTER_UI_H

void ControlCenterGUI::changedSlot()
{
    emit changed();
}

void ControlCenterGUI::listView_selectionChanged()
{
    bool empty = listView->selectedItems ().isEmpty();
    changeShareBtn->setDisabled(empty );
    removeShareBtn->setDisabled(empty );

}

#endif // SIMPLE_CONTROLCENTER_UI_H
