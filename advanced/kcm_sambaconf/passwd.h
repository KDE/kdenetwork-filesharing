/***************************************************************************
                          passwdfile.h  -  description
                             -------------------
    begin                : Tue June 6 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KSambaPlugin.                                          *
 *                                                                            *
 *  KSambaPlugin is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KSambaPlugin is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with KSambaPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#ifndef PASSWD_H
#define PASSWD_H

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <qstringlist.h>

QStringList getUnixUsers()
{
  QStringList list;

  struct passwd* p;

  while ((p = getpwent()))
  {
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
    list.append(QString(g->gr_name));
  }

  endgrent();

  list.sort();

  return list;
}


#endif