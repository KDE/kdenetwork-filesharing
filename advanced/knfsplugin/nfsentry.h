/***************************************************************************
                          nfsentry.h  -  description
                             -------------------
    begin                : Don Apr 25 2002
    copyright            : (C) 2002 by Jan Sch�fer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KNfsPlugin.                                          *
 *                                                                            *
 *  Foobar is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  Foobar is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with Foobar; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#ifndef NFSENTRY_H
#define NFSENTRY_H


/**
  *@author Jan Sch�fer
  */

#include <qstring.h>
#include <qptrlist.h>


class NFSEntry;

class NFSHost
{
public:
  NFSHost(const QString &, NFSEntry *);
  ~NFSHost();

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

  QString _name;

  QString paramString();

  QString toString();

  bool setName(const QString &);
  QString name() const;

  void setEntry(NFSEntry* entry);
  NFSEntry* getEntry();

protected:
  void parseParamsString(const QString &);
  void setParam(const QString &);
  void initParams();

  NFSEntry* nfsEntry;
};

typedef QPtrList<NFSHost>  HostList;

typedef QPtrListIterator<NFSHost> HostIterator;

class NFSFile;

class NFSEntry {
public:
	NFSEntry(const QString &, NFSFile *file);
	~NFSEntry();

  void addHost(NFSHost * host);
  void removeHost(NFSHost * host);
  NFSHost* getHostByName(const QString & name);
  HostIterator getHosts();

  QString getPath();
  void setPath(const QString &);

  QString toString();

  NFSFile* getNFSFile();

protected:
  HostList _hosts;
  QString _path;
  NFSFile* nfsFile;
};

#endif