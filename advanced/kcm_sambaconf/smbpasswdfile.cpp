/***************************************************************************
                          smbpasswdfile.cpp  -  description
                             -------------------
    begin                : Tue June 6 2002
    copyright            : (C) 2002 by Jan Schäfer
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

#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>

#include <kurl.h>
#include <kdebug.h>

#include "sambafile.h"
#include "smbpasswdfile.h"


QStringList SambaUserList::getUserNames()
{
  QStringList list;

  SambaUser *user;
  for ( user = first(); user; user = next() )
  {
     list.append(user->name);
  }

  return list;
}


SmbPasswdFile::SmbPasswdFile(const KURL & url)
{
  _url = url;
}

SmbPasswdFile::~SmbPasswdFile()
{
}

/**
 * Returns a list of all users stored in
 * the smbpasswd file
 **/
SambaUserList SmbPasswdFile::getSambaUserList()
{
  QFile f(_url.path());

  SambaUserList list;

  if ( f.open(IO_ReadOnly) )
  {
    QTextStream t( &f );
    QString s;
    while ( !t.eof() )
    {
      s = t.readLine().stripWhiteSpace();

      // Ignore comments
      if (s.left(1)=="#")
         continue;

      QStringList l = QStringList::split(":",s);

      SambaUser* user = new SambaUser(l[0],l[1].toInt());
      list.append(user);
    }
    f.close();
  }

  return list;
}

/**
 * Tries to add the passed user to the smbpasswd file
 * returns true if successful otherwise false
 **/
bool SmbPasswdFile::addUser(const SambaUser &)
{
}

/**
 * Tries to remove the passed user from the smbpasswd file
 * returns true if successful otherwise false
 **/
bool SmbPasswdFile::removeUser(const SambaUser &)
{
}

/**
 * Returns the Url of the smbpasswd file
 * specified in the [global] section of
 * the smb.conf file.
 * If there is no entry in the [global] section
 * it tries to guess the location.
 **/
KURL SmbPasswdFile::getUrlFromSambaFile(const SambaFile *file)
{
}

