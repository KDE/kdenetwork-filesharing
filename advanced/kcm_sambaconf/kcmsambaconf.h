/***************************************************************************
                          kcmsambaconf.h  -  description
                            -------------------
    begin                : Mon Apr  8 13:35:56 CEST 2002
    copyright            : (C) 2002 by Christian Nitschkowski
    email                : segfault_ii@web.de
    copyright            : (C) 2002-2003 by Jan Schäfer
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
*  along with KSambaPlugin; if not, write to the Free Software                     *
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
*                                                                            *
******************************************************************************/

#ifndef KCMSAMBACONF_H_
#define KCMSAMBACONF_H_

#include <kcmodule.h>
#include <kcminterface.h>
#include <share.h>
#include <qptrlist.h>
#include <qlistview.h>


class SambaShare;
class SambaFile;
class QPixmap;
/**
* A QListViewItem which holds a SambaShare object
**/
class ShareListViewItem : public QListViewItem
{
public:
  ShareListViewItem(QListView * parent, SambaShare* share);

  SambaShare* getShare() const;
  void setShare(SambaShare* share);
  void updateShare();

protected:
  SambaShare* _share;
  QPixmap createPropertyPixmap();
};


class KJanusWidget;
class QLineEdit;
class QCheckBox;
class QSpinBox;
class DictManager;
class SmbConfConfigWidget;

class KcmSambaConf: public KCModule
{
  Q_OBJECT
  public:
    KcmSambaConf(QWidget *parent = 0L, const char *name = 0L);
    virtual ~KcmSambaConf();

    void load(const QString &);
    void save();
    void defaults();
    int buttons();
    QString quickHelp() const;

  public slots:
    void configChanged();

    void editShare();
    void addShare();
    void removeShare();

    void editPrinter();
    void addPrinter();
    void removePrinter();

    void editShareDefaults();
    void editPrinterDefaults();


  protected:
    /**
    * The path of the smb.conf file
    **/
    QString _smbconf;
    SambaFile* _sambaFile;

    DictManager* _dictMngr;

    void init();
    void loadUserTab();
    void saveUserTab();
    bool getSocketBoolValue( const QString & str, const QString & name );
    int getSocketIntValue( const QString & str, const QString & name );
    QString socketOptions();
    void setComboIndexToValue(QComboBox* box, const QString & value, SambaShare* share);

    void loadBaseSettings(SambaShare*s=0L);
    void loadSecurity(SambaShare* );
    void loadLogging(SambaShare* );
    void loadTuning(SambaShare* );
    void loadDomain(SambaShare* );
    void loadWins(SambaShare* );
    void loadPrinting(SambaShare* );
    void loadFilenames(SambaShare* );
    void loadLocking(SambaShare* );
    void loadCharset(SambaShare*);
    void loadLogon(SambaShare* );
    void loadSocket(SambaShare* );
    void loadSSL(SambaShare* );
    void loadProtocol(SambaShare* );
    void loadWinbind(SambaShare* );
    void loadNetbios(SambaShare* );
    void loadVFS(SambaShare* );
    void loadBrowsing(SambaShare* );
    void loadCommands(SambaShare*);
    void loadMisc(SambaShare* );
    void loadDebug(SambaShare* );
    void loadLDAP(SambaShare*);

    void initAdvancedTab();

    void createSmbConfigWidget();
    
  protected slots:
    void addSambaUserBtnClicked();
    void removeSambaUserBtnClicked();
    void sambaUserPasswordBtnClicked();
    void slotMouseButtonPressed(int,QListViewItem*,const QPoint &,int);
    void joinADomainBtnClicked();
    void nullPasswordsEnabled(bool);
    void loadBtnClicked();
    void loadCanceled(const QString & msg);
    void fillFields(); 
    void slotSpecifySmbConf(const QString &);
    
  private:
    KcmInterface* _interface;
    KJanusWidget* _janus;
    SmbConfConfigWidget* m_smbConfConfigWidget;
};

#endif
