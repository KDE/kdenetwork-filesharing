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
#include <qstringlist.h>
#include <qptrlist.h>

class SambaConfigFile;
class QStringList;

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
  const QString& getName() const;


  /**
   * Sets the name of the share
   * returns false if the name already exists and the testWetherExists
   * parameter is set to true
   **/
  bool setName(const QString & name, bool testWetherExists=true);

  /**
   * Tests wether or not the given option is supported by Samba.
   * It does this by testing the output of testparm for this option
   **/
  bool optionSupported(const QString & name);
  
  /**
   * Returns the value of the given parameter
   * if the parameter doesn't exists, the
	 * global and then the default value is returned.
   * @param name the name of the parameter
   * @param gobalValue if the gobal value should be returend
   * @param defaultValue if the default value should be returned or a null string
   **/
  QString getValue(const QString & name, bool globalValue=true, bool defaultValue=true);

  /**
   * Same as above but for boolean parameters
   * Don't use defaultValue = false with this function
   * because you can't distinguish an unset parameter
   * from a false parameter
   **/
  bool getBoolValue(const QString & name, bool globalValue=true, bool defaultValue=true);

  /**
   * Returns the value from the [globals] section
   **/
	QString getGlobalValue(const QString & name, bool defaultValue=true);

  /**
   * Sets the parameter name to the given value
   * @param value the value of the parameter
   * @param name the name of the parameter
   * @param globalValue if true then the value is only set if it is different to the global value
   * @param defaultValue if true then the value is only set if it is different to the default value
   *                     if globalValue and defaultValue is true then the value is set when a global value
   *                     exists and it it is different to it. If no global value exists then it is only
   *                     set if different to the default value.
   **/
  void setValue(const QString & name,const QString & value, bool globalValue=true, bool defaultValue=true);

  /**
   * Same as above but for boolean parameters
   **/
  void setValue(const QString & name, bool value, bool globalValue=true, bool defaultValue=true);

  /**
   * Same as above but for integer parameters
   **/
  void setValue(const QString & name, int value, bool globalValue=true, bool defaultValue=true);

  /**
   * Returns the default value of the parameter
   * @param name the name of the parameter
   **/
	QString getDefaultValue(const QString & name);

  /**
   * Same as above but for booleans
   **/
  bool getDefaultBoolValue(const QString & name);

  /**
 	 * Returns the default synonym for the given parameter
 	 * if no synonym exists the original parameter in lower
 	 * case is returned
 	 **/
	QString getSynonym(const QString & name) const;

  /**
   * Returns the comments of the share
   * e.g. the text above the [...] section
   **/
  QStringList getComments();

  /**
   * Sets the comments for the share
   * e.g. the text above the [...] section
   **/
  void setComments(const QStringList & commentList);
  
  /**
   * Sets the comments for the passed option
   **/
  void setComments(const QString & name, const QStringList & commentList);

  /**
   * Returns the comments of the passed option
   **/
  QStringList getComments(const QString & name);

  /**
   * Returns true if the passed option has comments
   * otherwise returns false
   **/
  bool hasComments(const QString & name);

  /**
   * Returns the list of all options
   * the order of the options is exactly the
   * order of the insertion of the options
   **/
  QStringList getOptionList();
  
	/**
   * Returns true if this share is a printer
   * that's if printable = true
   **/
  bool isPrinter();

  /**
   * Returns true if the share name is
   * global, printers or homes
   **/
  bool isSpecialSection();

protected:
	/**
   * The name of the share
   * could be also printers, global and homes
   **/
	QString _name;
  SambaConfigFile* _sambaFile;

  /**
   * This attribute stores all option comments.
   * the comments which stood above the option name
   * are stored in this QStringList
   **/
  QDict<QStringList> _commentList;

  /**
   * The comments for this share
   **/
  QStringList _comments;

  /**
   * An extra list which holds
   * all stored options
   * You might say, hey for what is this ?
   * We have them already stored in the QDict.
   * That's right, but there is a problem :
   * QDict doesn't preserve the order of
   * the inserted items, but when saving
   * the options back to the smb.conf
   * we want to have exactly the same order
   * so this QStringList is only for saving
   * the order of the options.
   * On the other side we need a very fast lookup
   * of the options, because we lookup very frequently
   * so this is the best way to do both.
   **/
  QStringList _optionList;

};

typedef QPtrList<SambaShare> SambaShareList;


#endif
