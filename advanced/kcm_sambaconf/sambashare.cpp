/***************************************************************************
                          sambashare.cpp  -  description
                            -------------------
    begin                : Mon Jun 12 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
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

#include <kdebug.h>
#include <qstringlist.h>

#include "sambafile.h"
#include "sambashare.h"
#include "common.h"

SambaShare::SambaShare(SambaConfigFile* sambaFile)
  : QDict<QString>(10,false)
{
  _sambaFile = sambaFile;
  setName("defaults");
  setAutoDelete(true);
}

SambaShare::SambaShare(const QString & name, SambaConfigFile* sambaFile)
  : QDict<QString>(10,false)
{
  _sambaFile = sambaFile;
  setName(name);
  setAutoDelete(true);
}

const QString& SambaShare::getName() const
{
  return _name;
}

bool SambaShare::setName(const QString & name, bool testWetherExists)
{

  if ( testWetherExists &&
      _sambaFile->find(name) &&
      _sambaFile->find(name) != this)
    return false;

  _name = name;
return true;
}

bool SambaShare::optionSupported(const QString & name) 
{
  QString defaultValue = _sambaFile->getDefaultValue(name);
//  kdDebug(5009) << name << " = " << defaultValue << " null : " << defaultValue.isNull() << endl;
  return ! defaultValue.isNull();
}

/**
* Returns the value of the given parameter
* if no value is set yet the default value
* will be returned.
**/
QString SambaShare::getValue(const QString & name, bool globalValue, bool defaultValue)
{
  QString synonym = getSynonym(name);

  QString* str = find(synonym);
  QString ret;

  if (str) {
    ret = *str;
  }     
  else
  if (globalValue)
    ret = getGlobalValue(synonym,defaultValue);
  else
  if (defaultValue)
    ret = getDefaultValue(synonym);


  if (name=="writable" || name=="write ok" || name=="writeable")
    ret = textFromBool( ! boolFromText(ret) );

  return ret;
}

bool SambaShare::getBoolValue(const QString & name, bool globalValue, bool defaultValue)
{
  return boolFromText(getValue(name,globalValue,defaultValue));
}


QString SambaShare::getGlobalValue(const QString & name, bool defaultValue)
{
  if (!_sambaFile)
    return getValue(name,false,defaultValue);

  SambaShare* globals = _sambaFile->find("global");

  QString s = globals->getValue(name,false,defaultValue);

  return s;
}


/**
* Returns the default synonym for the given parameter
* if no synonym exists the original parameter in lower
* case is returned
**/
QString SambaShare::getSynonym(const QString & name) const
{
  QString lname = name.lower().stripWhiteSpace();

  if (lname == "browsable") return "browseable";
  if (lname == "allow hosts") return "hosts allow";
  if (lname == "auto services") return "preload";
  if (lname == "casesignames") return "case sensitive";
  if (lname == "create mode") return "create mask";
  if (lname == "debuglevel") return "log level";
  if (lname == "default") return "default service";
  if (lname == "deny hosts") return "hosts deny";
  if (lname == "directory") return "path";
  if (lname == "directory mode") return "directory mask";
  if (lname == "exec") return "preexec";
  if (lname == "group") return "force group";
  if (lname == "lock dir") return "lock directory";
  if (lname == "min passwd length") return "min password length";
  if (lname == "only guest") return "guest only";
  if (lname == "prefered master") return "preferred master";
  if (lname == "print ok") return "printable";
  if (lname == "printcap") return "printcap name";
  if (lname == "printer") return "printer name";
  if (lname == "protocol") return "max protocol";
  if (lname == "public") return "guest ok";
  if (lname == "writable") return "read only";
  if (lname == "write ok") return "read only";
  if (lname == "read only") return "read only";
  if (lname == "root") return "root directory";
  if (lname == "root") return "root dir";
  if (lname == "timestamp logs") return "debug timestamp";
  if (lname == "user") return "username";
  if (lname == "users") return "username";
  if (lname == "idmap uid") return "winbind uid";
  if (lname == "idmap gid") return "winbind gid";
  if (lname == "vfs object") return "vfs objects";


  return lname;
}

void SambaShare::setValue(const QString & name, const QString & value, bool globalValue, bool defaultValue)
{
  QString synonym = getSynonym(name);

  QString newValue = value;

  if (newValue.isNull())
    newValue = "";
  
  if (getName().lower() == "global")
    globalValue = false;

  if (name=="writable" || name=="write ok" || name=="writeable")
  {
    synonym = "read only";
    newValue = textFromBool(!boolFromText(value));
  }

  QString global = "";

  if (globalValue && !hasComments(synonym))
  {
    global = getGlobalValue(synonym, false);

    if ( newValue.lower() == global.lower() )
    {
      remove(synonym);
      _optionList.remove(synonym);
      return;
    }
  }

  // If the option has a comment we don't remove
  // it if the value is equal to the default value.
  // That's because the author of the option has thought about it.
  if (defaultValue && global.isEmpty() && !hasComments(synonym))
  {
    if ( newValue.stripWhiteSpace().lower() == getDefaultValue(synonym).stripWhiteSpace().lower() )
    {
      kdDebug(5009) << getName() << " global: " << global << " remove " << synonym << endl;
      remove(synonym);
      _optionList.remove(synonym);
      return;
    } 
    
  }

  if (!find(synonym))
  {
    _optionList.append(synonym);
  }
    
  replace(synonym,new QString(newValue));
}

void SambaShare::setValue(const QString & name, bool value, bool globalValue, bool defaultValue)
{
  setValue(name,textFromBool(value),globalValue, defaultValue);
}

void SambaShare::setValue(const QString & name, int value, bool globalValue, bool defaultValue)
{
  setValue(name,QString::number(value),globalValue, defaultValue);
}

/**
* Returns the default value of the parameter
**/
QString SambaShare::getDefaultValue(const QString & name)
{
  QString defaultValue = _sambaFile->getDefaultValue(name);
  if (defaultValue.isNull())
    defaultValue = "";

  return defaultValue;
}

bool SambaShare::getDefaultBoolValue(const QString & name)
{

  return boolFromText(getDefaultValue(name));
}

/**
* Sets the comments for the passed option
**/
void SambaShare::setComments(const QString & name, const QStringList & commentList)
{
  // Only add inempty lists
  if (commentList.empty())
    return;

  QString synonym = getSynonym(name);

  _commentList.replace(name,new QStringList(commentList));
}

/**
* Returns the comments of the passed option
**/
QStringList SambaShare::getComments(const QString & name)
{
  QStringList* list = _commentList.find(getSynonym(name));

  if (list)
    return QStringList(*list);
  else
    return QStringList();
}


bool SambaShare::hasComments(const QString & name)
{
  return 0L != _commentList.find(getSynonym(name));
}

/**
* Returns the comments of the share
* e.g. the text above the [...] section
**/
QStringList SambaShare::getComments()
{
  return _comments;
}

/**
* Sets the comments for the share
* e.g. the text above the [...] section
**/
void SambaShare::setComments(const QStringList & commentList)
{
  _comments = commentList;
}

QStringList SambaShare::getOptionList()
{
  return _optionList;
}

/**
* Returns true if this share is a printer
**/
bool SambaShare::isPrinter()
{
  QString* str = find("printable");

  if (!str)
      str = find("print ok");

  return str!=0;
}

/**
* Returns true if the share name is
* global, printers or homes
**/
bool SambaShare::isSpecialSection()
{
  if ( _name.lower() == "global" ||
      _name.lower() == "printers" ||
      _name.lower() == "homes" )
    return true;
  else
    return false;
}
