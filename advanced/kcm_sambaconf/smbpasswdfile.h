/***************************************************************************
                          smbpasswdfile.h  -  description
                             -------------------
    begin                : Tue June 6 2002
    copyright            : (C) 2002 by Jan Sch�fer
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#ifndef SMBPASSWDFILE_H
#define SMBPASSWDFILE_H


/**
  *@author Jan Sch�fer
  */

#include <qstring.h>

class KURL;
class SambaFile;


/**
 * Simple class to store a Samba User
 **/
class SambaUser
{
public:
  SambaUser(const QString & aName = QString::null, int anUid = -1) {name = aName; uid = anUid;};

  QString name;
  int uid;
};

/**
 * A QPtrList of SambaUser
 **/
class SambaUserList : public QPtrList<SambaUser>
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
class SmbPasswdFile {
public: 
	SmbPasswdFile(const KURL &);
	~SmbPasswdFile();

  /**
   * Returns a list of all users stored in
   * the smbpasswd file
   **/
  SambaUserList getSambaUserList();

  /**
   * Tries to add the passed user to the smbpasswd file
   * returns true if successful otherwise false
   **/
  bool addUser(const SambaUser &);

  /**
   * Tries to remove the passed user from the smbpasswd file
   * returns true if successful otherwise false
   **/
  bool removeUser(const SambaUser &);

  /**
   * Returns the Url of the smbpasswd file
   * specified in the [global] section of
   * the smb.conf file.
   * If there is no entry in the [global] section
   * it tries to guess the location.
   **/
  static KURL getUrlFromSambaFile(const SambaFile *file);
protected:
  KURL _url;
};

#endif