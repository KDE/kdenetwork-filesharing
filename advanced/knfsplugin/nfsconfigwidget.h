/***************************************************************************
                          nfsconfigwidget.  -  description
                             -------------------
    begin                : Don Apr 25 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NFSCONFIGWIDGET_H
#define NFSCONFIGWIDGET_H

#include <qwidget.h>
#include <qstring.h>

class NFSConfigWidget : public QWidget
{
Q_OBJECT
public:
	NFSConfigWidget(QWidget* parent);
  ~NFSConfigWidget();
  
  // the /etc/exports path
  QString exports;

protected slots:
  void specifyExportsSlot();

  
signals:
	void exportsSpecified(QString);
};

#endif