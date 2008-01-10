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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA  *
 *                                                                            *
 ******************************************************************************/


#ifndef KCMINTERFACE_H
#define KCMINTERFACE_H

#include <kiconloader.h>
#include <k3process.h>


#include "ui_kcminterface.h"

class KcmInterface : public QWidget, public Ui::KcmInterface
{
Q_OBJECT

public:
  KcmInterface(QWidget* parent);
  void init();

public slots:
  void changedSlot();
  void securityLevelCombo_activated( int i );
  void updateSecurityLevelHelpLbl();
  void shareRadio_clicked();
  void userRadio_clicked();
  void serverRadio_clicked();
  void domainRadio_clicked();
  void adsRadioClicked();
  void KUrlLabel1_leftClickedURL();
  void lmAnnounceCombo_activated( int i);
  void allowGuestLoginsChk_toggled( bool b);
  void mapToGuestCombo_activated( int i);

signals:
 void changed();

};

#endif // KCMINTERFACE_H
