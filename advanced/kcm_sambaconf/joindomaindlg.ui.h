/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#ifndef KCM_SAMBACONF_JOINDOMAINDLG_UI_H
#define KCM_SAMBACONF_JOINDOMAINDLG_UI_H

void JoinDomainDlg::accept() {
    if (passwordEdit->text() != verifyEdit->text()) {
        KMessageBox::sorry(this, i18n("Sorry"), i18n("You entered two different passwords. Please try again."));
       return;
    }
    
    QDialog::accept();
}

#endif // KCM_SAMBACONF_JOINDOMAINDLG_UI_H
