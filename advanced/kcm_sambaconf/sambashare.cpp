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

#include "sambafile.h"
#include "sambashare.h"

SambaShare::SambaShare(SambaConfigFile* sambaFile)
	: QDict<QString>(10,false)
{
  _sambaFile = sambaFile;
	_name = "";
  setAutoDelete(true);
}

SambaShare::SambaShare(const QString & name, SambaConfigFile* sambaFile)
	: QDict<QString>(10,false)
{
  _sambaFile = sambaFile;
	setName(name);
  setAutoDelete(true);
}

QString SambaShare::getName() const
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

/**
 * Returns the value of the given parameter
 * if no value is set yet the default value
 * will be returned.
 **/
QString SambaShare::getValue(const QString & name, bool globalValue=true, bool defaultValue=true)
{
	QString synonym = getSynonym(name);

  QString* str = find(synonym);
  QString ret;

  if (str)
  	 ret = *str;
  else
  if (globalValue)
		 ret = getGlobalValue(synonym,defaultValue);
	else
  if (defaultValue)
     ret = getDefaultValue(synonym);
	else
  	 ret = "";

	if (name == "read only")
  	 ret = SambaFile::textFromBool( ! SambaFile::boolFromText(ret) );

  return ret;
}

bool SambaShare::getBoolValue(const QString & name, bool globalValue=true, bool defaultValue=true)
{
	return SambaFile::boolFromText(getValue(name,globalValue,defaultValue));
}


QString SambaShare::getGlobalValue(const QString & name, bool defaultValue=true)
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
	if (lname == "writable") return "writeable";
	if (lname == "write ok") return "writeable";
	if (lname == "read only") return "writeable";
	if (lname == "root") return "root directory";
	if (lname == "root") return "root dir";
	if (lname == "timestamp logs") return "debug timestamp";
  if (lname == "user") return "username";
  if (lname == "users") return "username";

  return lname;
}

void SambaShare::setValue(const QString & name, const QString & value, bool globalValue=true, bool defaultValue=true)
{
  QString synonym = getSynonym(name);

  QString newValue = value;

  if (name=="read only")
  {
		synonym = "writeable";
    newValue = SambaFile::textFromBool(!SambaFile::boolFromText(value));
  }

  QString global = "";

	if (globalValue)
  {
		global = getGlobalValue(synonym, false);

  	if ( newValue == global )
    {
			remove(synonym);
			return;
    }
  }

  if (defaultValue && global=="")
  {
  	if ( newValue == getDefaultValue(synonym) )
    {
			remove(synonym);
    	return;
   	}
  }

	replace(synonym,new QString(newValue));
}

void SambaShare::setValue(const QString & name, bool value, bool globalValue=true, bool defaultValue=true)
{
	setValue(name,SambaFile::textFromBool(value),globalValue, defaultValue);
}

void SambaShare::setValue(const QString & name, int value, bool globalValue=true, bool defaultValue=true)
{
	setValue(name,QString::number(value),globalValue, defaultValue);
}

/**
 * Returns the default value of the parameter
 **/
QString SambaShare::getDefaultValue(const QString & name)
{
  return _sambaFile->getDefaultValue(name);
}

bool SambaShare::getDefaultBoolValue(const QString & name)
{

	return SambaFile::boolFromText(getDefaultValue(name));
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
