/***************************************************************************
                          nfspropsdlgplugin.cpp  -  description
                             -------------------
    begin                : Don Apr 25 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KNfsPlugin.                                          *
 *                                                                            *
 *  KNfsPlugin is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KNfsPlugin is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with KNfsPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <kfiledialog.h>

#include <klocale.h>

#include "nfsconfigwidget.h"

NFSConfigWidget::NFSConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	exports = "";

  QVBoxLayout *layout = new QVBoxLayout(this,5);

  QLabel *lbl = new QLabel(i18n("<p>The NFS configuration file <strong>'exports'</strong>" \
  															" could not be found!</p>" \
                   						  "Make sure you have NFS installed.\n\n"), this);

	QHBoxLayout *hbox = new QHBoxLayout(this);
	QPushButton *btn = new QPushButton(i18n("Specify location"), this);
  connect(btn, SIGNAL(pressed()), this, SLOT( specifyExportsSlot()));

  btn->setDefault(false);
  btn->setAutoDefault(false);

  hbox->addStretch();
  hbox->addWidget(btn);

	layout->addWidget(lbl);
	layout->addLayout(hbox);
  layout->addStretch();

}

NFSConfigWidget::~NFSConfigWidget()
{
}

void NFSConfigWidget::specifyExportsSlot()
{

	exports = KFileDialog::getOpenFileName("/",
   					"exports|NFS conf. file\n"
        		"*|All files",0,i18n("Get exports location"));

  emit exportsSpecified(exports);
}

#include "nfsconfigwidget.moc"
