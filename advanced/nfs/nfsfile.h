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

#ifndef NFSFILE_H
#define NFSFILE_H


#include <kurl.h>
#include "nfsentry.h"

typedef QPtrList<NFSEntry> EntryList;
typedef QPtrListIterator<NFSEntry> EntryIterator;

class QTextStream;

class NFSFile {
public:
  NFSFile(const KURL & , bool readonly = true);
  ~NFSFile();

  void addEntry(NFSEntry *);
  void removeEntry(NFSEntry *);
  bool hasEntry(NFSEntry *);
  NFSEntry* getEntryByPath(const QString &);
  bool removeEntryByPath(const QString &);
  EntryIterator getEntries();

  bool save();
  bool saveTo(const QString& fileName);
  void saveTo(QTextStream * stream);  
  bool load();

  bool restartNFSServer;


protected:
  KURL _url;
  bool _readonly;
  EntryList _entries;
  NFSLineList _lines;
};

#endif
