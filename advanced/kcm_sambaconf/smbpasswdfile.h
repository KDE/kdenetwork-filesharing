/***************************************************************************
                          smbpasswdfile.h  -  description
                             -------------------
    begin                : Tue June 6 2002
    copyright            : (C) 2002 by Jan Schäfer
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

#ifndef SMBPASSWDFILE_H
#define SMBPASSWDFILE_H


/**
  *@author Jan Schäfer
  */

#include <qstring.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <kurl.h>

class SambaFile;
class K3Process;

/**
 * Simple class to store a Samba User
 **/
class SambaUser
{
public:
  SambaUser(const QString & aName = QString::null, int anUid = -1) {name = aName; uid = anUid;}	//krazy:exclude=nullstrassign for old broken gcc

  QString name;
  int uid;
  int gid;
  bool isUserAccount;
  bool hasNoPassword;
  bool isDisabled;
  bool isWorkstationTrustAccount;
};

/**
 * A QPtrList of SambaUser
 **/
class SambaUserList : public Q3PtrList<SambaUser>
{
public:
  QStringList getUserNames();
};

/**
 * This class represents the SAMBA smbpasswd file.
 * It provides :
 * - getting a list of all SAMBA users
 * - adding a new user -> uses smbpasswd program
 * - removing an existing user -> uses smbpasswd program
 **/
class SmbPasswdFile : public QObject
{
Q_OBJECT
public: 
  SmbPasswdFile();
	SmbPasswdFile(const KUrl &);
	~SmbPasswdFile();

  void setUrl(const KUrl &);
  
  /**
   * Returns a list of all users stored in
   * the smbpasswd file
   **/
  SambaUserList getSambaUserList();

  /**
   * Calls smbpasswd with the given arguments
   **/
  bool executeSmbpasswd(const QStringList & args);
  
  /**
   * Tries to add the passed user to the smbpasswd file
   * returns true if successful otherwise false
   **/
  bool addUser(const SambaUser &, const QString & password);

  /**
   * Tries to remove the passed user from the smbpasswd file
   * returns true if successful otherwise false
   **/
  bool removeUser(const SambaUser &);

  /**
   * Tries to change the password of the passed user
   * if it fails returns false otherwise true
   **/
  bool changePassword(const SambaUser &, const QString & password);
  
  bool enableUser(const SambaUser &);
  
  bool disableUser(const SambaUser &);
  
  bool setNoPassword(const SambaUser &);
  
  bool setMachineTrustAccount(const SambaUser &);

  bool joinADomain(const QString &, const QString &, const QString &, const QString &);
  
  /**
   * Returns the Url of the smbpasswd file
   * specified in the [global] section of
   * the smb.conf file.
   * If there is no entry in the [global] section
   * it tries to guess the location.
   **/
  static KUrl getUrlFromSambaFile(const SambaFile *file);
protected:
  KUrl _url;
  QString _smbpasswdOutput;

protected slots:
  void smbpasswdStdOutReceived(K3Process*,char*,int);
};

#endif
