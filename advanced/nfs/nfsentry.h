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
  Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#ifndef NFSENTRY_H
#define NFSENTRY_H


#include <qstring.h>
#include <q3ptrlist.h>


class NFSHost
{
public:
  NFSHost(const QString & hostString);
  ~NFSHost();
  NFSHost();

  bool readonly;
  bool sync;
  bool secure;
  bool wdelay;
  bool hide;
  bool subtreeCheck;
  bool secureLocks;
  bool allSquash;
  bool rootSquash;

  int anonuid;
  int anongid;

  QString name;

  QString paramString() const;
  QString toString() const;
  bool isPublic() const;
  
  /** returns a copy of this host */
  NFSHost* copy() const;
protected:
  void parseParamsString(const QString &);
  void setParam(const QString &);
  void initParams();
};



typedef Q3PtrList<NFSHost> HostList;
typedef Q3PtrListIterator<NFSHost> HostIterator;

class NFSLine {
public:  
  virtual QString toString() const = 0;
};

typedef Q3PtrList<NFSLine> NFSLineList;
typedef Q3PtrListIterator<NFSLine> NFSLineIterator;

class NFSEmptyLine : public NFSLine {
public:
  virtual QString toString() const { return QString::fromLatin1("\n"); }
  virtual ~NFSEmptyLine() {};
};

class NFSComment : public NFSLine {
public:
  NFSComment(const QString & s) { comment = s; }
  virtual ~NFSComment() {};
  QString comment;
  virtual QString toString() const { return comment; }
};

class NFSEntry : public NFSLine {
public:
  NFSEntry(const QString & path);
  virtual ~NFSEntry();

  void addHost(NFSHost * host);
  void removeHost(NFSHost * host);
  NFSHost* getHostByName(const QString & name) const;
  NFSHost* getPublicHost() const;
  HostIterator getHosts() const;
  
  /** Creates a deep copy of this entry */
  NFSEntry * copy();
  
  /** clears all hosts and makes a deep copy of 
   *  the given entry
   */
  void copyFrom(NFSEntry* entry);
  
  /** removes all hosts */
  void clear();
  QString path() const;
  void setPath(const QString &);

  virtual QString toString() const;

protected:
  HostList _hosts;
  QString _path;
};

#endif
