/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void SocketOptionsDlg::setShare( SambaShare * share )
{
    assert(share);
    
    _share = share;
    
    QString s = _share->getValue("socket options");
    
    s = s.simplifyWhiteSpace();
    
    // The string s has now the form e.g. :
    // "OPTION1=1 OPTION2=0 OPTION3=2234 OPTION4"
    
SO_KEEPALIVEChk->setChecked(getBoolValue( s, "SO_KEEPALIVE") );
SO_REUSEADDRChk->setChecked( getBoolValue( s, "SO_REUSEADDR") );
SO_BROADCASTChk->setChecked( getBoolValue( s, "SO_BROADCAST") );
TCP_NODELAYChk->setChecked( getBoolValue( s, "TCP_NODELAY") );
IPTOS_LOWDELAYChk->setChecked( getBoolValue( s, "IPTOS_LOWDELAY") );
IPTOS_THROUGHPUTChk->setChecked( getBoolValue( s, "IPTOS_THROUGHPUT") );
SO_SNDBUFChk->setChecked( getBoolValue( s, "SO_SNDBUF") );
SO_RCVBUFChk->setChecked( getBoolValue( s, "SO_RCVBUF") );
SO_SNDLOWATChk->setChecked( getBoolValue( s, "SO_SNDLOWAT") );
SO_RCVLOWATChk->setChecked( getBoolValue( s, "SO_RCVLOWAT") );

SO_SNDBUFSpin->setValue( getIntValue( s, "SO_SNDBUF") );
SO_RCVBUFSpin->setValue( getIntValue( s, "SO_RCVBUF") );
SO_SNDLOWATSpin->setValue( getIntValue( s, "SO_SNDLOWAT") );
SO_RCVLOWATSpin->setValue( getIntValue( s, "SO_RCVLOWAT") );
   
}



bool SocketOptionsDlg::getBoolValue( const QString & str, const QString & name )
{
    QString s = str;
    int i = s.find(name ,0,false);
		
    if (i > -1)
    {
	s = s.remove(0,i+1+QString(name).length());
	if ( s.startsWith("=") )
	{
	    s = s.remove(0,1);
	    if ( s.startsWith("0"))
		return false;
	    else
		return true;
	}
	else
	    return true;
    }
    
    return false;
}

int SocketOptionsDlg::getIntValue( const QString & str, const QString & name )
{
    QString s = str;
    int i = s.find(name ,0,false);
		
    if (i > -1)
    {
	s = s.remove(0,i+1+QString(name).length());
	if ( s.startsWith("=") )
	{
	    s = s.remove(0,1);
	    
	    i = s.find(" ");
	    if (i < 0)
	        i = s.length();
	    else
	        i++;
	    
	    s = s.left( i );
	    
	    return s.toInt();
	}
	else
	    return 0;
    }
    
    return 0;
}