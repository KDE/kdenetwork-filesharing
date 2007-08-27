/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#ifndef SIMPLE_GROUPCONFIGGUI_UI_H
#define SIMPLE_GROUPCONFIGGUI_UI_H

void GroupConfigGUI::listBox_selectionChanged( Q3ListBoxItem * i)
{
    removeBtn->setEnabled(i);
}

#endif // SIMPLE_GROUPCONFIGGUI_UI_H
