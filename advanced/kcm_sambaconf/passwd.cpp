/***************************************************************************
                          passwd.cpp  -  description
                             -------------------
    begin                : Mon Apr  8 13:35:56 CEST 2002
    copyright            : (C) 2002 by Christian Nitschkowski
    email                : segfault_ii@web.de
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
 *  along with KSambaPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/


#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "passwd.h"

UnixUserList getUnixUserList()
{
  UnixUserList list;

  struct passwd* p;

  while ((p = getpwent()))
  {
    if (!p) continue;

    UnixUser *u = new UnixUser();
    u->name = p->pw_name;
    u->uid = p->pw_uid;
    list.append(u);
  }

  endpwent();

  list.sort();

  return list;
}

QStringList getUnixUsers()
{
  QStringList list;

  struct passwd* p;

  while ((p = getpwent()))
  {
    if (p)
       list.append(QString(p->pw_name));
  }

  endpwent();

  list.sort();

  return list;
}

QStringList getUnixGroups()
{
  QStringList list;

  struct group* g;

  while ((g = getgrent()))
  {
    if (g)
       list.append(QString(g->gr_name));
  }

  endgrent();

  list.sort();

  return list;
}

int getUserUID(const QString & name)
{
  struct passwd* p;

  p = getpwnam(name);

  if (p)
     return p->pw_uid;

  return -1;
}

int getUserGID(const QString & name)
{
  struct passwd* p;

  p = getpwnam(name);

  if (p)
    return p->pw_gid;

  return -1;
}

int getGroupGID(const QString & name)
{
  struct group* g;

  g = getgrnam(name);

  if (g)
    return g->gr_gid;

  return -1;
}
