/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
  Copyright (c) 2011 Rodrigo Belem <rclbelem@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include <QFileInfo>
#include <QStringList>
#include <QStandardItemModel>
#include <QDBusInterface>
#include <QDBusReply>

#include <kvbox.h>
#include <kuser.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <ksambashare.h>
#include <ksambasharedata.h>
#include <kmessagebox.h>
#include <KDE/KPluginFactory>
#include <KDE/KPluginLoader>

#include "sambausershareplugin.h"
#include "model.h"
#include "delegate.h"

K_PLUGIN_FACTORY(SambaUserSharePluginFactory, registerPlugin<SambaUserSharePlugin>();)
K_EXPORT_PLUGIN(SambaUserSharePluginFactory("fileshare_propsdlgplugin"))

SambaUserSharePlugin::SambaUserSharePlugin(QObject *parent, const QList<QVariant> &args)
    : KPropertiesDialogPlugin(qobject_cast<KPropertiesDialog *>(parent))
    , url()
    , shareData()
{
    url = properties->kurl().path(KUrl::RemoveTrailingSlash);
    if (url.isEmpty()) {
        return;
    }

    QFileInfo pathInfo(url);
    if (!pathInfo.permission(QFile::ReadUser | QFile::WriteUser)) {
        return;
    }

    KGlobal::locale()->insertCatalog("kfileshare");

    KVBox *vbox = new KVBox();
    properties->addPage(vbox, i18n("&Share"));
    properties->setFileSharingPage(vbox);

    if (!QFile::exists("/usr/sbin/smbd")
        && !QFile::exists("/usr/local/sbin/smbd")) {

        QWidget *widget = new QWidget(vbox);
        QVBoxLayout *vLayout = new QVBoxLayout(widget);
        vLayout->setAlignment(Qt::AlignJustify);
        vLayout->setSpacing(KDialog::spacingHint());
        vLayout->setMargin(0);

        vLayout->addWidget(new QLabel(i18n("Samba is not installed on your system."), widget));

#ifdef SAMBA_INSTALL
        KPushButton *btn = new KPushButton(i18n("Install Samba..."), widget);
        btn->setDefault(false);
        vLayout->addWidget(btn);
        connect(btn, SIGNAL(clicked()), SLOT(installSamba()));
#endif

        // align items on top
        vLayout->addStretch();

        return;
    }

    QWidget *widget = new QWidget(vbox);
    propertiesUi.setupUi(widget);

    QList<KSambaShareData> shareList = KSambaShare::instance()->getSharesByPath(url);

    if (!shareList.isEmpty()) {
        shareData = shareList.at(0); // FIXME: using just the first in the list for a while
    }

    setupModel();
    setupViews();
    load();

    connect(propertiesUi.sambaChk, SIGNAL(toggled(bool)), this, SLOT(toggleShareStatus(bool)));
    connect(propertiesUi.sambaChk, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    connect(propertiesUi.sambaNameEdit, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));
    connect(propertiesUi.sambaNameEdit, SIGNAL(textChanged(QString)), this, SLOT(checkShareName(QString)));
    connect(propertiesUi.sambaAllowGuestChk, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(changed()));

    for (int i = 0; i < model->rowCount(); ++i) {
        propertiesUi.tableView->openPersistentEditor(model->index(i, 1, QModelIndex()));
    }
}

SambaUserSharePlugin::~SambaUserSharePlugin()
{
}

void SambaUserSharePlugin::installSamba()
{
    unsigned int xid = 0;
    QStringList packages;
    packages << SAMBA_PACKAGE_NAME;
    QString interaction("show-confirm-install,show-progress");

    QDBusInterface device("org.freedesktop.PackageKit", "/org/freedesktop/PackageKit",
                          "org.freedesktop.PackageKit.Modify");
    if (!device.isValid()) {
        KMessageBox::sorry(qobject_cast<KPropertiesDialog *>(this),
                i18n("<qt><strong>Samba could not be installed.</strong><br />Please, check if kpackagekit is properly installed</qt>"));
        return;
    }
    QDBusReply<int> reply = device.call("InstallPackageNames", xid, packages, interaction);
}

void SambaUserSharePlugin::setupModel()
{
    model = new UserPermissionModel(shareData, this);
}

void SambaUserSharePlugin::setupViews()
{
    propertiesUi.tableView->setModel(model);
    propertiesUi.tableView->setSelectionMode(QAbstractItemView::NoSelection);
    propertiesUi.tableView->setItemDelegate(new UserPermissionDelegate(this));
}

void SambaUserSharePlugin::load()
{
    bool guestAllowed = false;
    bool sambaShared = KSambaShare::instance()->isDirectoryShared(url);

    propertiesUi.sambaChk->setChecked(sambaShared);
    toggleShareStatus(sambaShared);
    if (sambaShared) {
        guestAllowed = (bool) shareData.guestPermission();
    }
    propertiesUi.sambaAllowGuestChk->setChecked(guestAllowed);

    propertiesUi.sambaNameEdit->setText(shareData.name());
}

void SambaUserSharePlugin::applyChanges()
{
    KSambaShareData::UserShareError result;

    if (propertiesUi.sambaChk->isChecked()) {
        if (shareData.setAcl(model->getAcl()) != KSambaShareData::UserShareAclOk) {
            return;
        }

        shareData.setName(propertiesUi.sambaNameEdit->text());

        shareData.setPath(url);

        KSambaShareData::GuestPermission guestOk(shareData.guestPermission());

        guestOk = (propertiesUi.sambaAllowGuestChk->isChecked() == false)
                  ? KSambaShareData::GuestsNotAllowed : KSambaShareData::GuestsAllowed;

        shareData.setGuestPermission(guestOk);

        result = shareData.save();
    } else if (KSambaShare::instance()->isDirectoryShared(url)) {
        result = shareData.remove();
    }
}

void SambaUserSharePlugin::toggleShareStatus(bool checked)
{
    propertiesUi.sambaNameEdit->setEnabled(checked);
    propertiesUi.sambaAllowGuestChk->setCheckable(checked);
    propertiesUi.tableView->setEnabled(checked);
    if (checked && propertiesUi.sambaNameEdit->text().isEmpty()) {
        propertiesUi.sambaNameEdit->setText(getNewShareName());
    }
}

void SambaUserSharePlugin::checkShareName(const QString &name)
{
    bool disableButton = false;

    if (name.isEmpty()) {
        disableButton = true;
    } else if (!KSambaShare::instance()->isShareNameAvailable(name)) {
        // There is another Share with the same name
        KMessageBox::sorry(qobject_cast<KPropertiesDialog *>(this),
                i18n("<qt>There is already a share with the name <strong>%1</strong>.<br /> Please choose another name.</qt>",
                    propertiesUi.sambaNameEdit->text()));
        propertiesUi.sambaNameEdit->selectAll();
        disableButton = true;
    }

    if (disableButton) {
        properties->enableButtonOk(false);
        propertiesUi.sambaNameEdit->setFocus();
        return;
    }

    if (!properties->isButtonEnabled(KPropertiesDialog::Ok)) {
        properties->enableButtonOk(true);
    }
}

QString SambaUserSharePlugin::getNewShareName()
{
    QString shareName = KUrl(url).fileName();

    if (!propertiesUi.sambaNameEdit->text().isEmpty()) {
        shareName = propertiesUi.sambaNameEdit->text();
    }

    // Windows could have problems with longer names
    shareName = shareName.left(12);

    return shareName;
}

#include "moc_sambausershareplugin.cpp"
