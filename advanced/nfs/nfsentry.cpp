/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <kdebug.h>

#include "nfsentry.h"

NFSHost::NFSHost(const QString & hostString)
{
  readonly = true;

  QString s = hostString;

  int l = s.find('(');
  int r = s.find(')');

  initParams();

  // get hostname
  if (l>=0)
    name = s.left(l);
  else
    name = s;
    
  kdDebug(5009) << "NFSHost: name='" << name << "'" << endl;    
  
  if (l>=0 && r>=0)
  {
    QString params = s.mid(l+1,r-l-1);

    parseParamsString(params);
  }
}

NFSHost::NFSHost() {
  initParams();
  name="";
}

NFSHost::~NFSHost()
{
}

/**
 * Set the parameters to their default values
 */
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

  anonuid = 65534;
  anongid = 65534;
}


void NFSHost::parseParamsString(const QString & s)
{

  if (s.isEmpty()) 
      return;

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
      rest = rest.mid(i+1);
    }

    setParam(p);
  }
  while (i>-1);

}

QString NFSHost::paramString() const
{
  QString s;

  if (!readonly) s+="rw,";
  if (!rootSquash) s+="no_root_squash,";
  if (!secure) s+="insecure,";
  if (!secureLocks) s+="insecure_locks,";
  if (!subtreeCheck) s+="no_subtree_check,";
  if (sync) 
    s+="sync,";
  else
    s+="async,";    
    
  if (!wdelay) s+="wdelay,";
  if (allSquash) s+="all_squash,";
  if (!hide) s+="nohide,";

  if (anongid!=65534)
     s+=QString("anongid=%1,").arg(anongid);

  if (anonuid!=65534)
     s+=QString("anonuid=%1,").arg(anonuid);

  // get rid of the last ','
  s.truncate(s.length()-1);

  return s;
}

QString NFSHost::toString() const
{
  QString s = name;

  s+='(';
  s+=paramString();
  s+=')';

  return s;

}

NFSHost* NFSHost::copy() const {
  NFSHost* result = new NFSHost();
  
  result->name = name;
  
  result->readonly = readonly;
  result->sync = sync;
  result->secure = secure;
  result->wdelay = wdelay;
  result->hide = hide;
  result->subtreeCheck = subtreeCheck;
  result->secureLocks = secureLocks;
  result->allSquash = allSquash;
  result->rootSquash = rootSquash;

  result->anonuid = anonuid;
  result->anongid = anongid;

  return result;  
}

bool NFSHost::isPublic() const {
  return name == "*";
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
     kdDebug(5009) << name << endl;

     QString value = p.mid(i+1);
     kdDebug(5009) << value << endl;

    if (name=="anongid")
       anongid = value.toInt();

    if (name=="anonuid")
       anonuid = value.toInt();
  }

}

NFSEntry::NFSEntry(const QString & path)
{
  _hosts.setAutoDelete(true);
  setPath(path);
}

NFSEntry::~NFSEntry()
{
}

void NFSEntry::clear() {
  _hosts.clear();
}

NFSEntry* NFSEntry::copy() {
  NFSEntry* result = new NFSEntry(path());
  result->copyFrom(this);
  return result;
}

void NFSEntry::copyFrom(NFSEntry* entry) {
  clear();
  HostIterator it = entry->getHosts();

  NFSHost* host;
  while ( (host = it.current()) != 0 )  {
    ++it;
    addHost(host->copy());
  }
}

QString NFSEntry::toString() const
{
  QString s = _path.stripWhiteSpace();

  if (_path.find(' ') > -1) {
    s = '"'+s+'"';
  }
  
  s += ' ';

  HostIterator it = getHosts();

  NFSHost* host;

  while ( (host = it.current()) != 0 )
  {
    ++it;
    s+= host->toString() ;
    if (it.current())
        s+= " \\\n\t ";
  }


  return s;
}

void NFSEntry::addHost(NFSHost * host)
{
  _hosts.append(host);
}

void NFSEntry::removeHost(NFSHost * host)
{
  _hosts.remove(host);
}

NFSHost* NFSEntry::getHostByName(const QString & name) const
{
  HostIterator it = getHosts();
  NFSHost* host;
  
  while ( (host = it.current()) != 0 )
  {
    ++it;

    if (host->name==name)
      return host;
  }

  return 0;
}

NFSHost* NFSEntry::getPublicHost() const
{
  NFSHost* result = getHostByName("*");
  if (result)
      return result;
      
  return getHostByName(QString::null);
}


HostIterator NFSEntry::getHosts() const
{
  return HostIterator(_hosts);
}

QString NFSEntry::path() const
{
  return _path;
}

void NFSEntry::setPath(const QString & path)
{
  _path = path;
}

