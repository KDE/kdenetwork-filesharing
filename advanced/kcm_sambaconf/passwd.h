/***************************************************************************
                          passwd.h  -  description
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

#include <qstringlist.h>
#include <qstring.h>
#include <qptrlist.h>



class UnixUser
{
public:
  QString name;
  int uid;
};

typedef QPtrList<UnixUser> UnixUserList;

UnixUserList getUnixUserList();
QStringList getUnixUsers();
QStringList getUnixGroups();
int getUserUID(const QString &);
int getUserGID(const QString &);
int getGroupGID(const QString &);
bool isUserInGroup(const QString &, const QString &);


#endif
