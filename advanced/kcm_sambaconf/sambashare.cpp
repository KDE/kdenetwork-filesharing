/***************************************************************************
                          sambashare.cpp  -  description
                             -------------------
    begin                : Mon Jun 12 2002
    copyright            : (C) 2002 by Jan Schäfer
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

#include "sambafile.h"

#include "sambashare.h"

SambaShare::SambaShare(SambaConfigFile* sambaFile)
	: QDict<QString>(10,false)
{
	setName(QString(""));
  setAutoDelete(true);
  _sambaFile = sambaFile;
}

SambaShare::SambaShare(const QString & name, SambaConfigFile* sambaFile)
	: QDict<QString>(10,false)
{
	setName(name);
  setAutoDelete(true);
  _sambaFile = sambaFile;
}

QString SambaShare::getName() const
{
	return _name;
}

void SambaShare::setName(const QString & name)
{
	_name = name;

}

/**
 * Returns the value of the given parameter
 * if no value is set yet the default value
 * will be returned.
 **/
QString SambaShare::getValue(const QString & name, bool globalValue=true, bool defaultValue=true) const
{
	QString synonym = getSynonym(name);
  
  QString* str = find(name);
  QString ret;

  if (!str && globalValue)
		 ret = getGlobalValue(name,defaultValue);
	else
  if (!str && defaultValue)
     ret = getDefaultValue(name);
	else
	if (!str)
  	 ret = "";

	if (name == "read only")
  	 ret = SambaFile::textFromBool( ! SambaFile::boolFromText(ret) );

  return ret;
}

bool SambaShare::getBoolValue(const QString & name, bool globalValue=true, bool defaultValue=true) const
{
	return SambaFile::boolFromText(getValue(name,globalValue,defaultValue));
}


QString SambaShare::getGlobalValue(const QString & name, bool defaultValue=true) const
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
	QString lname = name.lower();

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

void SambaShare::setValue(const QString & name, const QString & value)
{
	QString synonym = getSynonym(name);

  QString newValue = value;

  if (name=="read only")
  {
		synonym = "writeable";
    newValue = SambaFile::textFromBool(!SambaFile::boolFromText(value));
  }


	replace(synonym,new QString(value));
}

void SambaShare::setValue(const QString & name, bool value)
{
	replace(name,new QString(SambaFile::textFromBool(value)));
}

/**
 * Returns the default value of the parameter
 **/
QString SambaShare::getDefaultValue(const QString & name) const
{
	QString lname = name.lower();

	if (lname == "available") return "yes";
	if (lname == "browseable") return "yes";
	if (lname == "browsable") return "yes";

  // Security options

	if (lname == "read only") return "yes";
	if (lname == "guest account") return "nobody";
	if (lname == "writeble") return "no";
	if (lname == "write ok") return "no";
	if (lname == "create mask") return "0744";
	if (lname == "force create mode") return "00";
	if (lname == "security mask") return "0777";
	if (lname == "force security mode") return "00";
	if (lname == "directory mask") return "0755";
	if (lname == "force directory mode") return "00";
	if (lname == "directory security mask") return "0777";
	if (lname == "force directory security mode") return "00";
	if (lname == "inherit permissions") return "no";
	if (lname == "guest ok") return "no";
	if (lname == "public") return "no";
	if (lname == "guest only") return "no";
	if (lname == "only user") return "no";

  // Logging options

	if (lname == "status") return "yes";
  
  // Tuning Options

	if (lname == "max connections") return "0";
	if (lname == "strict sync") return "no";
	if (lname == "sync always") return "no";
	if (lname == "write cache size") return "0";
  
  // Filename Handling
  
	if (lname == "default case") return "lower";
	if (lname == "case sensitive") return "no";
	if (lname == "preserve case") return "yes";
	if (lname == "short preserve case") return "yes";
	if (lname == "mangle case") return "no";
	if (lname == "mangling char") return "~";
	if (lname == "hide dot files") return "yes";
	if (lname == "hide unreadable") return "no";
	if (lname == "delete veto files") return "no";
	if (lname == "map system") return "no";
	if (lname == "map hidden") return "no";
	if (lname == "map archive") return "yes";
	if (lname == "mangled names") return "yes";

  // Locking options

  if (lname == "blocking locks") return "yes";
	if (lname == "fake oplocks") return "no";
  if (lname == "locking") return "yes";
	if (lname == "oplocks") return "yes";
  if (lname == "level2 oplocks") return "yes";
	if (lname == "oplock contention limit") return "2";
  if (lname == "posix locking") return "yes";
	if (lname == "strict locking") return "no";
  if (lname == "share modes") return "yes";
  
  // Miscellaneous options

	if (lname == "preexec close") return "no";
	if (lname == "root preexec close") return "no";
	if (lname == "fstype") return "NTFS";
	if (lname == "set directory") return "no";
	if (lname == "wide links") return "yes";
	if (lname == "follow symlinks") return "yes";
	if (lname == "delete readonly") return "no";
	if (lname == "dos filemode") return "no";
	if (lname == "dos filetimes") return "no";
	if (lname == "dos filetime resolution") return "no";
	if (lname == "fake directory create times") return "no";
	
  // VFS options

  if (lname == "msdfs root") return "no";
  
  // [global]
  
  // Security options

  if (lname == "security") return "USER";
  if (lname == "encrypt passwords") return "no";
  if (lname == "update encrypted") return "no";
  if (lname == "allow trusted domains") return "yes";
  if (lname == "alternate permissions") return "no";
  if (lname == "min passwd length") return "5";
  if (lname == "map to guest") return "Never";
  if (lname == "null passwords") return "no";
  if (lname == "obey pam restrictions") return "no";
  if (lname == "smb passwd file") return "/etc/samba/smbpasswd";
  if (lname == "pam password change") return "no";
  if (lname == "passwd program") return "/bin/passwd";
  if (lname == "passwd chat debug") return "no";
  if (lname == "password level") return "0";
  if (lname == "username level") return "0";
  if (lname == "unix password sync") return "no";
  if (lname == "restrict anonymous") return "no";
  if (lname == "lanman auth") return "yes";
  if (lname == "use rhosts") return "no";
  
  // Logging Options

  if (lname == "log level") return "0";
  if (lname == "syslog") return "1";
  if (lname == "syslog only") return "no";
  if (lname == "max log size") return "5000";
  if (lname == "timestamp logs") return "yes";
  if (lname == "debug hires timestamp") return "no";
  if (lname == "debug pid") return "no";
  if (lname == "debug uid") return "no";
  
  // Protocol Options

  if (lname == "protocol") return "NT1";
  if (lname == "max protocol") return "NT1";
  if (lname == "large readwrite") return "no";
  if (lname == "min protocol") return "CORE";
  if (lname == "read bmpx") return "no";
  if (lname == "read raw") return "yes";
  if (lname == "write raw") return "yes";
  if (lname == "nt smb support") return "yes";
  if (lname == "nt pipe support") return "yes";
  if (lname == "nt acl support") return "yes";
  if (lname == "announce version") return "4.5";
  if (lname == "announce as") return "NT";
  if (lname == "max mux") return "50";
  if (lname == "max xmit") return "65535";
  if (lname == "name resolve order") return "lmhosts host wins bcast";
  if (lname == "max packet") return "65535";
  if (lname == "max ttl") return "259200";
  if (lname == "max wins ttl") return "518400";
  if (lname == "min wins ttl") return "21600";
  if (lname == "time server") return "no";
  
  // Tuning options

  if (lname == "change notify timeout") return "60";
  if (lname == "deadtime") return "0";
  if (lname == "getwd cache") return "yes";
  if (lname == "keepalive") return "300";
  if (lname == "lpq cache time") return "10";
  if (lname == "max smbd processes") return "0";
  if (lname == "max disk size") return "0";
  if (lname == "max open files") return "10000";
  if (lname == "read size") return "16384";
  if (lname == "socket options") return "TCP_NODELAY";
  if (lname == "stat cache size") return "50";
  
  // Printing options

  if (lname == "total print jobs") return "0";
  if (lname == "load printers") return "yes";
  if (lname == "printcap name") return "/etc/printcap";
  if (lname == "printing") return "bsd";
  if (lname == "print command") return "lpr -r -P%p %s";
  if (lname == "lpq command") return "lpq -P%p";
  if (lname == "lprm command") return "lprm -P%p %j";
  if (lname == "show add printer wizard") return "yes";
  
  // Filename Handling

  if (lname == "strip dot") return "no";
  if (lname == "mangled stack") return "50";
  if (lname == "stat cache") return "yes";
  
  // Domain options

  if (lname == "machine password timeout") return "604800";

	// Logon options

  if (lname == "logon path") return "\\\\%N\\%U\\profile";
  if (lname == "logon home") return "\\\\%N\\%U";
  if (lname == "domain logons") return "no";

  // Browse options

  if (lname == "os level") return "20";
  if (lname == "lm announce") return "Auto";
  if (lname == "lm interval") return "60";
  if (lname == "preferred master") return "Auto";
  if (lname == "local master") return "yes";
  if (lname == "domain master") return "Auto";
  if (lname == "browse list") return "yes";
  if (lname == "enhanced browsing") return "yes";
  
  // WINS options

  if (lname == "dns proxy") return "yes";
  if (lname == "wins proxy") return "no";
  if (lname == "wins support") return "no";
  
  // Locking options

  if (lname == "kernel oplocks") return "yes";
  if (lname == "oplock break wait time") return "0";
  
  // Miscellaneous options

  if (lname == "lock dir") return "/var/lib/samba";
  if (lname == "pid dir") return "/var/run/samba";
  if (lname == "socket address") return "0.0.0.0";
  if (lname == "homedir map") return "auto.home";
  if (lname == "time offset") return "0";
  if (lname == "hide local users") return "no";
  
  // VFS options

  if (lname == "host msdfs") return "no";
  
  // Windbind options 

  if (lname == "template homedir") return "/home/%D/%U";
  if (lname == "template shell") return "/bin/false";
  if (lname == "winbind separator") return "\\";
  if (lname == "winbind cache time") return "15";

  return "";

}

bool SambaShare::getDefaultBoolValue(const QString & name) const
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
