/***************************************************************************
                          dictmanager.h  -  description
                             -------------------
    begin                : Wed Jan 1 2003
    copyright            : (C) 2003 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KSambaPlugin.                                          *
 *                                                                            *
 *  KSambaPlugin is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KSambaPlugin is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with KSambaPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA  *
 *                                                                            *
 ******************************************************************************/

#ifndef _DICTMANAGER_H_
#define _DICTMANAGER_H_

class SambaShare;
class QLineEdit;
class QCheckBox;
class QSpinBox;
class QComboBox;
class QString;
class SambaShare;
class QStringList;

/**
 * @author Jan Schäfer
 **/
class DictManager : public QObject
{
Q_OBJECT
  public :
    DictManager(SambaShare *share);
    virtual ~DictManager();
    
    void add(const QString &, QLineEdit*);
    void add(const QString &, QCheckBox*);
    void add(const QString &, KURLRequester*);
    void add(const QString &, QSpinBox*);
    void add(const QString &, QComboBox*, QStringList*);
              
    void load(SambaShare* share, bool globalValue=true, bool defaultValue=true);
    void save(SambaShare* share, bool globalValue=true, bool defaultValue=true);

  protected :
    Q3Dict<QLineEdit> lineEditDict;
    Q3Dict<QCheckBox> checkBoxDict;
    Q3Dict<KURLRequester> urlRequesterDict;
    Q3Dict<QSpinBox> spinBoxDict;
    Q3Dict<QComboBox> comboBoxDict;
    Q3Dict<QStringList> comboBoxValuesDict;
    
    SambaShare* _share;
    
    void handleUnsupportedWidget(const QString &, QWidget*);
    void loadComboBoxes(SambaShare*, bool, bool);
    
  protected slots:
    void changedSlot();
    
  signals:
    void changed();  
};
 
#endif
