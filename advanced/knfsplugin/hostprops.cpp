#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './hostprops.ui'
**
** Created: Die Mai 7 20:42:31 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "hostprops.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "hostprops.ui.h"

/* 
 *  Constructs a HostProps which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
HostProps::HostProps( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "HostProps" );
    resize( 380, 328 ); 
    setCaption( tr2i18n( "Form1" ) );
    HostPropsLayout = new QVBoxLayout( this, 0, 6, "HostPropsLayout"); 

    propertiesGrp = new QGroupBox( this, "propertiesGrp" );
    propertiesGrp->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, propertiesGrp->sizePolicy().hasHeightForWidth() ) );
    propertiesGrp->setMinimumSize( QSize( 180, 0 ) );
    propertiesGrp->setFrameShape( QGroupBox::Box );
    propertiesGrp->setFrameShadow( QGroupBox::Sunken );
    propertiesGrp->setTitle( tr2i18n( "Host Properties" ) );
    propertiesGrp->setColumnLayout(0, Qt::Vertical );
    propertiesGrp->layout()->setSpacing( 6 );
    propertiesGrp->layout()->setMargin( 11 );
    propertiesGrpLayout = new QVBoxLayout( propertiesGrp->layout() );
    propertiesGrpLayout->setAlignment( Qt::AlignTop );

    TextLabel1 = new QLabel( propertiesGrp, "TextLabel1" );
    TextLabel1->setText( tr2i18n( "Name / Adress :" ) );
    propertiesGrpLayout->addWidget( TextLabel1 );

    nameEdit = new QLineEdit( propertiesGrp, "nameEdit" );
    nameEdit->setFrameShape( QLineEdit::LineEditPanel );
    nameEdit->setFrameShadow( QLineEdit::Sunken );
    QWhatsThis::add( nameEdit, tr2i18n( "<b>Name / Adress field</b>\n"
"<p>\n"
"Here you can enter the host name or adress.<br>\n"
"The host may be specified in a number of ways: \n"
"<p>\n"
"<i>single host</i>\n"
"<p>\n"
" This is the most common format. You may specify a host either by an abbreviated name recognized be the resolver, the fully qualified domain name, or an IP address. \n"
"</p>\n"
"\n"
"<i>netgroups</i>\n"
"<p>\n"
" NIS netgroups may be given as @group. Only the host part of each netgroup members is consider in checking for membership. Empty host parts or those containing a single dash (-) are ignored. \n"
"</p>\n"
"\n"
"<i>wildcards</i>\n"
"<p>\n"
" Machine names may contain the wildcard characters * and ?. This can be used to make the exports file more compact; for instance, *.cs.foo.edu matches all hosts in the domain cs.foo.edu. However, these wildcard characters do not match the dots in a domain name, so the above pattern does not include hosts such as a.b.cs.foo.edu. \n"
"</p>\n"
"\n"
"<i>IP networks</i>\n"
"<p>\n"
" You can also export directories to all hosts on an IP (sub-) network simultaneously. This is done by specifying an IP address and netmask pair as address/netmask where the netmask can be specified in dotted-decimal format, or as a contiguous mask length (for example, either `/255.255.252.0' or `/22' appended to the network base address result in identical subnetworks with 10 bits of host). \n"
"</p>" ) );
    propertiesGrpLayout->addWidget( nameEdit );

    publicChk = new QCheckBox( propertiesGrp, "publicChk" );
    publicChk->setText( tr2i18n( "Public access" ) );
    QWhatsThis::add( publicChk, tr2i18n( "<b>Public access</b>\n"
"<p>\n"
"If you check this, the host adress will be a single wildcard , which means public access.\n"
"This is just the same as if you would enter a wildcard in the adress field.\n"
"</p>" ) );
    propertiesGrpLayout->addWidget( publicChk );
    HostPropsLayout->addWidget( propertiesGrp );

    Layout10 = new QHBoxLayout( 0, 0, 6, "Layout10"); 

    GroupBox7 = new QGroupBox( this, "GroupBox7" );
    GroupBox7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, GroupBox7->sizePolicy().hasHeightForWidth() ) );
    GroupBox7->setTitle( tr2i18n( "Options" ) );
    GroupBox7->setColumnLayout(0, Qt::Vertical );
    GroupBox7->layout()->setSpacing( 6 );
    GroupBox7->layout()->setMargin( 11 );
    GroupBox7Layout = new QVBoxLayout( GroupBox7->layout() );
    GroupBox7Layout->setAlignment( Qt::AlignTop );

    readOnlyChk = new QCheckBox( GroupBox7, "readOnlyChk" );
    readOnlyChk->setText( tr2i18n( "Writable" ) );
    QWhatsThis::add( readOnlyChk, tr2i18n( "<b>Writable</b>\n"
"<p>\n"
"Allow both read and write requests on this NFS volume.\n"
"</p>\n"
"<p>\n"
"The default is to disallow any request which changes the filesystem\n"
"</p>" ) );
    GroupBox7Layout->addWidget( readOnlyChk );

    secureChk = new QCheckBox( GroupBox7, "secureChk" );
    secureChk->setText( tr2i18n( "Insecure" ) );
    QWhatsThis::add( secureChk, tr2i18n( "<b>Insecure</b>\n"
"<p>\n"
"If this option is checked, it isn't required that requests originate on an internet port less than IPPORT_RESERVED (1024).\n"
"</p>\n"
"<p>\n"
"If unsure leave it unchecked.\n"
"</p>" ) );
    GroupBox7Layout->addWidget( secureChk );

    syncChk = new QCheckBox( GroupBox7, "syncChk" );
    syncChk->setText( tr2i18n( "Sync" ) );
    QWhatsThis::add( syncChk, tr2i18n( "<b>Sync</b>\n"
"<p>\n"
"This option requests that all file writes be committed to disc before the write request completes. This is required for complete safety of data in the face of a server crash, but incurs a performance hit.\n"
"</p>\n"
"<p>\n"
"The default is to allow the server to write the data out whenever it is ready. \n"
"</p>" ) );
    GroupBox7Layout->addWidget( syncChk );

    wdelayChk = new QCheckBox( GroupBox7, "wdelayChk" );
    wdelayChk->setEnabled( TRUE );
    wdelayChk->setText( tr2i18n( "No wdelay" ) );
    QWhatsThis::add( wdelayChk, tr2i18n( "<b>No wdelay</b>\n"
"<p>\n"
"This option only has effect if sync is also set. The NFS server will normally delay committing a write request to disc slightly if it suspects that another related write request may be in progress or may arrive soon. This allows multiple write requests to be committed to disc with the one operation which can improve performance. If an NFS server received mainly small unrelated requests, this behaviour could actually reduce performance, so no wdelay is available to turn it off. </p>" ) );
    GroupBox7Layout->addWidget( wdelayChk );

    hideChk = new QCheckBox( GroupBox7, "hideChk" );
    hideChk->setText( tr2i18n( "No hide" ) );
    QWhatsThis::add( hideChk, tr2i18n( "<b>No hide</b>\n"
"<p>\n"
"This option is based on the option of the same name provided in IRIX NFS. Normally, if a server exports two filesystems one of which is mounted on the other, then the client will have to mount both filesystems explicitly to get access to them. If it just mounts the parent, it will see an empty directory at the place where the other filesystem is mounted. That filesystem is \"hidden\". \n"
"</p>\n"
"<p>\n"
"Setting the nohide option on a filesystem causes it not to be hidden, and an appropriately authorised client will be able to move from the parent to that filesystem without noticing the change. \n"
"</p>\n"
"<p>\n"
"However, some NFS clients do not cope well with this situation as, for instance, it is then possible for two files in the one apparent filesystem to have the same inode number. \n"
"</p>\n"
"<p>\n"
"The nohide option is currently only effective on single host exports. It does not work reliably with netgroup, subnet, or wildcard exports. \n"
"</p>\n"
"<p>\n"
"This option can be very useful in some situations, but it should be used with due care, and only after confirming that the client system copes with the situation effectively. \n"
"</p>" ) );
    GroupBox7Layout->addWidget( hideChk );

    subtreeChk = new QCheckBox( GroupBox7, "subtreeChk" );
    subtreeChk->setText( tr2i18n( "No subtree check" ) );
    QWhatsThis::add( subtreeChk, tr2i18n( "<b>No subtree check</b>\n"
"<p>\n"
"This option disables subtree checking, which has mild security implications, but can improve reliability is some circumstances. \n"
"</p>\n"
"<p>\n"
"If a subdirectory of a filesystem is exported, but the whole filesystem isn't then whenever a NFS request arrives, the server must check not only that the accessed file is in the appropriate filesystem (which is easy) but also that it is in the exported tree (which is harder). This check is called the subtree_check. \n"
"</p>\n"
"<p>\n"
"In order to perform this check, the server must include some information about the location of the file in the \"filehandle\" that is given to the client. This can cause problems with accessing files that are renamed while a client has them open (though in many simple cases it will still work). \n"
"</p>\n"
"<p>\n"
"subtree checking is also used to make sure that files inside directories to which only root has access can only be accessed if the filesystem is exported with no_root_squash (see below), even the file itself allows more general access. \n"
"</p>\n"
"<p>\n"
"As a general guide, a home directory filesystem, which is normally exported at the root and may see lots of file renames, should be exported with subtree checking disabled. A filesystem which is mostly readonly, and at least doesn't see many file renames (e.g. /usr or /var) and for which subdirectories may be exported, should probably be exported with subtree checks enabled. \n"
"</p>" ) );
    GroupBox7Layout->addWidget( subtreeChk );

    secureLocksChk = new QCheckBox( GroupBox7, "secureLocksChk" );
    secureLocksChk->setText( tr2i18n( "Insecure locks" ) );
    QWhatsThis::add( secureLocksChk, tr2i18n( "<b>Insecure locks</b>\n"
"<p>\n"
"This option tells the NFS server not to require authentication of locking requests (i.e. requests which use the NLM protocol). Normally the NFS server will require a lock request to hold a credential for a user who has read access to the file. With this flag no access checks will be performed. \n"
"</p>\n"
"<p>\n"
"Early NFS client implementations did not send credentials with lock requests, and many current NFS clients still exist which are based on the old implementations. Use this flag if you find that you can only lock files which are world readable. \n"
"</p>" ) );
    GroupBox7Layout->addWidget( secureLocksChk );
    Layout10->addWidget( GroupBox7 );

    GroupBox3 = new QGroupBox( this, "GroupBox3" );
    GroupBox3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, GroupBox3->sizePolicy().hasHeightForWidth() ) );
    GroupBox3->setTitle( tr2i18n( "User mapping" ) );
    GroupBox3->setColumnLayout(0, Qt::Vertical );
    GroupBox3->layout()->setSpacing( 6 );
    GroupBox3->layout()->setMargin( 11 );
    GroupBox3Layout = new QVBoxLayout( GroupBox3->layout() );
    GroupBox3Layout->setAlignment( Qt::AlignTop );

    allSquashChk = new QCheckBox( GroupBox3, "allSquashChk" );
    allSquashChk->setText( tr2i18n( "All squash" ) );
    QWhatsThis::add( allSquashChk, tr2i18n( "<b>All squash</b>\n"
"<p>\n"
"Map all uids and gids to the anonymous user. Useful for NFS-exported public FTP directories, news spool directories, etc. </p>" ) );
    GroupBox3Layout->addWidget( allSquashChk );

    rootSquashChk = new QCheckBox( GroupBox3, "rootSquashChk" );
    rootSquashChk->setText( tr2i18n( "No root squash" ) );
    QWhatsThis::add( rootSquashChk, tr2i18n( "<b>No root squash</b>\n"
"<p>\n"
"Turn of root squashing. This option is mainly useful for diskless clients. \n"
"</p>\n"
"<i>root squashing</i>\n"
"<p>\n"
"Map requests from uid/gid 0 to the anonymous uid/gid. Note that this does not apply to any other uids that might be equally sensitive, such as user bin. \n"
"</p>" ) );
    GroupBox3Layout->addWidget( rootSquashChk );

    Layout6 = new QHBoxLayout( 0, 0, 6, "Layout6"); 

    TextLabel1_2 = new QLabel( GroupBox3, "TextLabel1_2" );
    TextLabel1_2->setText( tr2i18n( "Anonym. UID" ) );
    QWhatsThis::add( TextLabel1_2, tr2i18n( "<b>Anonym. UID/GID</b> <p> These options explicitly set the uid and gid of the anonymous account. This option is primarily useful for PC/NFS clients, where you might want all requests appear to be from one user. </p>" ) );
    Layout6->addWidget( TextLabel1_2 );

    anonuidEdit = new QLineEdit( GroupBox3, "anonuidEdit" );
    anonuidEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)0, 0, 0, anonuidEdit->sizePolicy().hasHeightForWidth() ) );
    anonuidEdit->setMaximumSize( QSize( 50, 50 ) );
    Layout6->addWidget( anonuidEdit );
    GroupBox3Layout->addLayout( Layout6 );

    Layout7 = new QHBoxLayout( 0, 0, 6, "Layout7"); 

    TextLabel2 = new QLabel( GroupBox3, "TextLabel2" );
    TextLabel2->setText( tr2i18n( "Anonym. GID" ) );
    QWhatsThis::add( TextLabel2, tr2i18n( "<b>Anonym. UID/GID</b> <p> These options explicitly set the uid and gid of the anonymous account. This option is primarily useful for PC/NFS clients, where you might want all requests appear to be from one user. </p>" ) );
    Layout7->addWidget( TextLabel2 );

    anongidEdit = new QLineEdit( GroupBox3, "anongidEdit" );
    anongidEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)0, 0, 0, anongidEdit->sizePolicy().hasHeightForWidth() ) );
    anongidEdit->setMaximumSize( QSize( 50, 50 ) );
    Layout7->addWidget( anongidEdit );
    GroupBox3Layout->addLayout( Layout7 );
    Layout10->addWidget( GroupBox3 );
    HostPropsLayout->addLayout( Layout10 );

    // signals and slots connections
    connect( publicChk, SIGNAL( toggled(bool) ), nameEdit, SLOT( setDisabled(bool) ) );
    connect( readOnlyChk, SIGNAL( clicked() ), this, SLOT( setModified() ) );
    connect( secureChk, SIGNAL( clicked() ), this, SLOT( setModified() ) );
    connect( syncChk, SIGNAL( clicked() ), this, SLOT( setModified() ) );
    connect( wdelayChk, SIGNAL( pressed() ), this, SLOT( setModified() ) );
    connect( subtreeChk, SIGNAL( pressed() ), this, SLOT( setModified() ) );
    connect( secureLocksChk, SIGNAL( pressed() ), this, SLOT( setModified() ) );
    connect( allSquashChk, SIGNAL( pressed() ), this, SLOT( setModified() ) );
    connect( rootSquashChk, SIGNAL( pressed() ), this, SLOT( setModified() ) );
    connect( anonuidEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( setModified() ) );
    connect( anongidEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( setModified() ) );
    connect( nameEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( setModified() ) );
    connect( hideChk, SIGNAL( clicked() ), this, SLOT( setModified() ) );
    connect( wdelayChk, SIGNAL( toggled(bool) ), wdelayChk, SLOT( setEnabled(bool) ) );
    connect( syncChk, SIGNAL( toggled(bool) ), wdelayChk, SLOT( setEnabled(bool) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
HostProps::~HostProps()
{
    // no need to delete child widgets, Qt does it all for us
}

#include "hostprops.moc"
