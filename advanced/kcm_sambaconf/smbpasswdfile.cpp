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
#include <kpassdlg.h>
#include <klocale.h>
#include <kprocess.h>

#include "sambafile.h"
#include "smbpasswdfile.h"
#include "passwd.h"


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


SmbPasswdFile::SmbPasswdFile() {
}


SmbPasswdFile::SmbPasswdFile(const KURL & url)
{
  setUrl(url);
}

SmbPasswdFile::~SmbPasswdFile()
{
}

void SmbPasswdFile::setUrl(const KURL & url) {
  _url = url;
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
      user->gid = getUserGID(l[0]);
      user->isUserAccount = l[4].contains('U');
      user->hasNoPassword = l[4].contains('N');;
      user->isDisabled = l[4].contains('D');;
      user->isWorkstationTrustAccount = l[4].contains('W');;
      list.append(user);
    }
    f.close();
  }

  return list;
}

bool SmbPasswdFile::executeSmbpasswd(const QStringList & args) {
  KProcess p;
  p << "smbpasswd" << args;

  connect( &p, SIGNAL(receivedStdout(KProcess*,char*,int)),
           this, SLOT(smbpasswdStdOutReceived(KProcess*,char*,int)));

  _smbpasswdOutput = "";

  bool result = p.start(KProcess::Block,KProcess::Stdout);

  if (result)
  {
    kdDebug(5009) << _smbpasswdOutput << endl;
  }

  return result;
}

/**
 * Tries to add the passed user to the smbpasswd file
 * returns true if successful otherwise false
 **/
bool SmbPasswdFile::addUser(const SambaUser & user,const QString & password)
{
  KProcess p;
  p << "smbpasswd" << "-a" << user.name;

  p << password;

  connect( &p, SIGNAL(receivedStdout(KProcess*,char*,int)),
           this, SLOT(smbpasswdStdOutReceived(KProcess*,char*,int)));

  _smbpasswdOutput = "";

  bool result = p.start(KProcess::Block,KProcess::Stdout);

  if (result)
  {
    kdDebug(5009) << _smbpasswdOutput << endl;
  }

  return result;
}

/**
 * Tries to remove the passed user from the smbpasswd file
 * returns true if successful otherwise false
 **/
bool SmbPasswdFile::removeUser(const SambaUser & user)
{
  QStringList l;
  l << "-x" << user.name;
  return executeSmbpasswd(l);
}

bool SmbPasswdFile::changePassword(const SambaUser & user, const QString & newPassword)
{
  return addUser(user,newPassword);
}


void SmbPasswdFile::smbpasswdStdOutReceived(KProcess *, char *buffer, int buflen)
{
  _smbpasswdOutput+=QString::fromLatin1(buffer,buflen);
}


/**
 * Returns the Url of the smbpasswd file
 * specified in the [global] section of
 * the smb.conf file.
 * If there is no entry in the [global] section
 * it tries to guess the location.
 **/
KURL SmbPasswdFile::getUrlFromSambaFile(const SambaFile * /*file*/)
{
  kdWarning() << "SmbPasswdFile::getUrlFromSambaFile unimplemeneted!" << endl;
  return KURL("");
}

bool SmbPasswdFile::enableUser(const SambaUser & user) {
  QStringList l;
  l << "-e" << user.name;
  return executeSmbpasswd(l);
}

bool SmbPasswdFile::disableUser(const SambaUser & user) {
  QStringList l;
  l << "-d" << user.name;
  return executeSmbpasswd(l);
}

bool SmbPasswdFile::setNoPassword(const SambaUser & user) {
  QStringList l;
  l << "-n" << user.name;
  return executeSmbpasswd(l);
}

bool SmbPasswdFile::setMachineTrustAccount(const SambaUser & user) {
  QStringList l;
  l << "-m" << user.name;
  return executeSmbpasswd(l);
}

bool SmbPasswdFile::joinADomain(const QString & domain, const QString & server,
      const QString & user, const QString & password) {
  QStringList l;
  l << "-j" << domain;
  l << "-r" << server;
  l << "-U" << user << "%" << password;
  return executeSmbpasswd(l);
}


#include "smbpasswdfile.moc"
