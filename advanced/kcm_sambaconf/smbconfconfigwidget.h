/***************************************************************************
                          smbconfconfigwidget.h  -  description
                             -------------------
    begin                : Tue May 16 2003
    copyright            : (C) 2003 by Jan Schäfer
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA  *
 *                                                                            *
 ******************************************************************************/

#ifndef SMBCONFCONFIGWIDGET_H
#define SMBCONFCONFIGWIDGET_H

#include <qwidget.h>

class SmbConfConfigWidget : public QWidget {
Q_OBJECT
public:
  SmbConfConfigWidget(QWidget*);
  
protected slots:
  void btnPressed();  
signals:
  void smbConfChoosed(const QString &);  
};


#endif
