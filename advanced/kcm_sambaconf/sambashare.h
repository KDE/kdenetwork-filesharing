/***************************************************************************
                          sambashare.h  -  description
                             -------------------
    begin                : Mon 12 2002
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

#ifndef SAMBASHARE_H
#define SAMBASHARE_H

#include <qdict.h>
#include <qstring.h>
#include <qptrlist.h>

class SambaConfigFile;

/**
 * A class which represents a Samba share
 * @author Jan Sch&auml;fer
 **/
class SambaShare :public QDict<QString>
{
public:

	/**
   * Creates a new SambaShare with an empty name
   **/
	SambaShare(SambaConfigFile* sambaFile);

	/**
   * Creates a new SambaShare with the given name
   * @param name the name of the share
   **/
  SambaShare(const QString & name, SambaConfigFile* sambaFile);

  /** 
   * Returns the name of the share
   **/
  QString getName() const;


  /**
   * Sets the name of the share
   **/
  void setName(const QString & name);

  /**
   * Returns the value of the given parameter
   * if the parameter doesn't exists, the
	 * global and then the default value is returned.
   * @param name the name of the parameter
   * @param gobalValue if the gobal value should be returend
   * @param defaultValue if the default value should be returned or a null string
   **/
  QString getValue(const QString & name, bool globalValue=true, bool defaultValue=true) const;

  /**
   * Same as above but for boolean parameters
   * Don't use defaultValue = false with this function
   * because you can't distinguish an unset paremeter
   * from a false parameter
   **/
  bool getBoolValue(const QString & name, bool globalValue=true, bool defaultValue=true) const;

  /**
   * Returns the value from the [globals] section
   **/
	QString getGlobalValue(const QString & name, bool defaultValue=true) const;

  /**
   * Sets the parameter name to the given value
   * @param value the value of the parameter
   * @param name the name of the parameter
   * @param globalValue if true then the value is only set if it is different from the global value
   * @param defaultValue if true then the value is only set if it is different from the default value
   *                     if globalValue and defaultValue is true then the value is set when a global value
   *                     exists and it it is different from it. If no global value exists then it is only
   *                     set if different from the default value.
   **/
  void setValue(const QString & name,const QString & value, bool globalValue=true, bool defaultValue=true);

  /**
   * Same as above but for boolean parameters
   **/
  void setValue(const QString & name, bool value, bool globalValue=true, bool defaultValue=true);

  /**
   * Returns the default value of the parameter
   * @param name the name of the parameter
   **/
	QString getDefaultValue(const QString & name) const;

  /**
   * Same as above but for booleans
   **/
  bool getDefaultBoolValue(const QString & name) const;

  /**
 	 * Returns the default synonym for the given parameter
 	 * if no synonym exists the original parameter in lower
 	 * case is returned
 	 **/
	QString getSynonym(const QString & name) const;

	/**
   * Returns true if this share is a printer
   **/
  bool isPrinter();

protected:
	/**
   * The name of the share
   * could be also printers, global and homes
   **/
	QString _name;
  SambaConfigFile* _sambaFile;


};

typedef QPtrList<SambaShare> SambaShareList;


#endif
