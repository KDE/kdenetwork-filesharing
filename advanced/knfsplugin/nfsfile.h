/***************************************************************************
                          nfsfile.h  -  description
                             -------------------
    begin                : Don Apr 25 2002
    copyright            : (C) 2002 by Jan Schäfer
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

#ifndef NFSFILE_H
#define NFSFILE_H


/**
  *@author Jan Schäfer
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
