/***************************************************************************
                          smbconfconfigwidget.cpp  -  description
                             -------------------
    begin                : Tue May 16 2003
    copyright            : (C) 2003 by Jan Sch�er
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

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
 *  along with KSambaPlugin; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#include <qfileinfo.h> 
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
 
#include <kapplication.h> 
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
 

#include "smbconfconfigwidget.h"

SmbConfConfigWidget::SmbConfConfigWidget(QWidget* parent) 
  : QWidget(parent,"configWidget")
{  

  QVBoxLayout *layout = new QVBoxLayout(this,5);

  QLabel *lbl = new QLabel(i18n("<p>The SAMBA configuration file <strong>'smb.conf'</strong>" \
                                " could not be found;</p>" \
                                "make sure you have SAMBA installed.\n\n"), this);

  QHBoxLayout *hbox = new QHBoxLayout(this);
  QPushButton *btn = new QPushButton(i18n("Specify Location"), this);
  connect(btn, SIGNAL(pressed()), this, SLOT( btnPressed()));

  btn->setDefault(false);
  btn->setAutoDefault(false);

  hbox->addStretch();
  hbox->addWidget(btn);

  layout->addWidget(lbl);
  layout->addLayout(hbox);
  layout->addStretch();
}

void SmbConfConfigWidget::btnPressed() {
  QString smbConf = KFileDialog::getOpenFileName("/",
            "smb.conf|Samba conf. File\n"
            "*|All Files",0,i18n("Get smb.conf Location"));

  if (smbConf.isEmpty())
		  return;
  if ( ! QFileInfo(smbConf).isReadable() ) {
    KMessageBox::sorry(this,i18n("<qt>The file <i>%1</i> could not be read.</qt>").arg(smbConf),i18n("Could Not Read File"));
    return;
  }

  KConfig config("ksambaplugin");
  
  config.setGroup("KSambaKonqiPlugin");
  config.writeEntry("smb.conf",smbConf);
  config.sync();
  
  emit smbConfChoosed(smbConf);

}

#include "smbconfconfigwidget.moc"
