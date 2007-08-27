/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

#ifndef NFS_HOSTPROPS_UI_H
#define NFS_HOSTPROPS_UI_H

void HostProps::setModified()
{
    emit modified();
}

#endif // NFS_HOSTPROPS_UI_H
