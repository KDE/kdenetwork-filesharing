/***************************************************************************
                          nfsfile.h  -  description
                             -------------------
    begin                : Don Apr 25 2002
    copyright            : (C) 2002 by Jan Sch�fer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NFSFILE_H
#define NFSFILE_H


/**
  *@author Jan Sch�fer
  */

#include <kurl.h>
#include "nfsentry.h"

typedef QPtrList<NFSEntry> EntryList;
typedef QPtrListIterator<NFSEntry> EntryIterator;

class NFSFile {
public: 
	NFSFile(const KURL & , bool = true);
	~NFSFile();

  void addEntry(NFSEntry *);
  void removeEntry(NFSEntry *);
  bool hasEntry(NFSEntry *);
  NFSEntry* getEntryByPath(const QString &);
  EntryIterator getEntries();
  /**
   * Try to find the path to the exports file
   * by testing several locations
   **/
  static QString guessPath();

  void save();
  void load();

  bool restartNFSServer;


protected:
  KURL _url;
  bool _readonly;
  EntryList _entries;

  QString getTempFileName();
};

#endif
