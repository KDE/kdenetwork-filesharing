/***************************************************************************
                          nfsentry.cpp  -  description
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

#include <kdebug.h>

#include "nfsentry.h"
#include "nfsfile.h"


NFSHost::NFSHost(const QString & hostString, NFSEntry * entry)
{
  nfsEntry = entry;

  if (nfsEntry)
     nfsEntry->addHost(this);

  readonly = true;

  QString s = hostString;

  int l = s.find("(",0);
  int r = s.find(")",0);

  initParams();

  if (l>=0 && r>=0)
  {
    QString params = s.mid(l+1,r-l-1);

    parseParamsString(params);
  }

  // Check if a parenthese exists
  if (l>=0)
    _name = s.left(l);
  else
    _name = s;

}

NFSHost::~NFSHost()
{
}

/**
 * Set the parameters to their default values
 **/
void NFSHost::initParams()
{
  readonly = true;
  sync = false;
  secure = true;
  wdelay = true;
  hide = true;
  subtreeCheck = true;
  secureLocks = true;
  allSquash = false;
  rootSquash = true;

  anonuid = -2;
  anongid = -2;
}


void NFSHost::setEntry(NFSEntry* entry)
{
  nfsEntry = entry;
}

NFSEntry* NFSHost::getEntry()
{
  return nfsEntry;
}


bool NFSHost::setName(const QString & name)
{
  NFSHost *testHost;

  // We don't want to have the same host twice
  if (nfsEntry)
  {
     testHost = nfsEntry->getHostByName(name);

     if (testHost && testHost != this)
         return false;
  }

  if (!nfsEntry)
      kdDebug() << "warning host with no entry !" << endl;

  _name = name;
  return true;
}

QString NFSHost::name() const
{
  return _name;
}



void NFSHost::parseParamsString(const QString & s)
{

  if (s=="") return;

  int i;

  QString rest = s;
  QString p;

  do
  {
    i = rest.find(",",0);

    if (i==-1)
      p = rest;
    else
    {
      p = rest.left( i );
      rest = rest.right( rest.length()-i-1 );

    }

    setParam(p);
  }
  while (i>-1);

}

QString NFSHost::paramString()
{
  QString s = "";

  if (!readonly) s+="rw,";
  if (!rootSquash) s+="no_root_squash,";
  if (!secure) s+="insecure,";
  if (!secureLocks) s+="insecure_locks,";
  if (!subtreeCheck) s+="no_subtree_check,";
  if (sync) s+="sync,";
  if (!wdelay) s+="wdelay,";
  if (allSquash) s+="all_squash,";
  if (!hide) s+="nohide,";

  if (anongid!=-2)
     s+=QString("anongid=%1,").arg(anongid);

  if (anonuid!=-2)
     s+=QString("anonuid=%1,").arg(anonuid);

  // get rid of the last ','
  s.truncate(s.length()-1);

  return s;
}

QString NFSHost::toString()
{
  QString s = "";

  s+=name();

  s+="(";
  s+=paramString();
  s+=")";

  return s;

}

void NFSHost::setParam(const QString & s)
{
  QString p = s.lower();


  if (p=="ro") {
     readonly = true;
     return; }

  if (p=="rw") {
     readonly = false;
     return; }

  if (p=="sync") {
     sync = true;
     return; }

  if (p=="async") {
     sync = false;
     return; }

  if (p=="secure") {
     secure = true;
     return; }

  if (p=="insecure") {
     secure = false;
     return; }

  if (p=="wdelay") {
     wdelay = true;
     return; }

  if (p=="no_wdelay") {
     wdelay = false;
     return; }

  if (p=="hide") {
     hide = true;
     return; }

  if (p=="nohide") {
     hide = false;
     return; }

  if (p=="subtree_check") {
     subtreeCheck = true;
     return; }

  if (p=="no_subtree_check") {
     subtreeCheck = false;
     return; }

  if (p=="secure_locks" ||
      p=="auth_nlm") {
     secureLocks = true;
     return; }

  if (p=="insecure_locks" ||
      p=="no_auth_nlm" ) {
     secureLocks = true;
     return; }

  if (p=="all_squash") {
     allSquash = true;
     return; }

  if (p=="no_all_squash") {
     allSquash = false;
     return; }

  if (p=="root_squash") {
     rootSquash = true;
     return; }

  if (p=="no_root_squash") {
     rootSquash = false;
     return; }

  int i = p.find("=",0);

  // get anongid or anonuid
  if (i>-1)
  {
     QString name = p.left(i).lower();
     kdDebug() << name << endl;

     QString value = p.right(p.length()-i-1);
     kdDebug() << value << endl;

    if (name=="anongid")
       anongid = value.toInt();

    if (name=="anonuid")
       anonuid = value.toInt();
  }

}

NFSEntry::NFSEntry(const QString & path, NFSFile* file)
{
//  _hosts.setAutoDelete(true);
  nfsFile = file;

  if (file)
      file->addEntry(this);
  else
     kdDebug() << "file == 0" << endl;

  setPath(path);
}

NFSEntry::~NFSEntry()
{
}

NFSFile* NFSEntry::getNFSFile()
{
  return nfsFile;
}

QString NFSEntry::toString()
{
  QString s = "";

  s+= _path+" ";

  HostIterator it = getHosts();

  NFSHost* host;

  while ( (host = it.current()) != 0 )
  {
    ++it;
    s+= " \\\n";
    s+= "\t "+host->toString() ;
  }


  return s;
}

void NFSEntry::addHost(NFSHost * host)
{
  host->setEntry(this);

  _hosts.append(host);
}

void NFSEntry::removeHost(NFSHost * host)
{
  host->setEntry(0L);

  _hosts.remove(host);
}

NFSHost* NFSEntry::getHostByName(const QString & name)
{
  HostIterator it = getHosts();

  NFSHost* host;

  while ( (host = it.current()) != 0 )
  {
    ++it;

    if (host->name()==name)
      return host;
  }

  return 0L;
}

HostIterator NFSEntry::getHosts()
{
  return HostIterator(_hosts);
}

QString NFSEntry::getPath()
{
  return _path;
}

void NFSEntry::setPath(const QString & path)
{
  _path = path.stripWhiteSpace();

  if (_path.right(1)=="/")
  	  _path.truncate(_path.length()-1);


}

