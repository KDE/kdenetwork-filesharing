/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
  Copyright (c) 2011 Rodrigo Belem <rclbelem@gmail.com>
  Copyright (c) 2015 Harald Sitter <sitter@kde.org>

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

#include <QDialogButtonBox>
#include <QFileInfo>
#include <QFrame>
#include <QPushButton>
#include <QStandardPaths>
#include <QStringList>
#include <QDebug>

#include <KMessageBox>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KSambaShare>
#include <KSambaShareData>

#include "sambausershareplugin.h"
#include "model.h"
#include "delegate.h"

K_PLUGIN_FACTORY(SambaUserSharePluginFactory, registerPlugin<SambaUserSharePlugin>();)
K_EXPORT_PLUGIN(SambaUserSharePluginFactory("fileshare_propsdlgplugin"))

// copied from kio/src/core/ksambashare.cpp, KSambaSharePrivate::isSambaInstalled()
static bool isSambaInstalled()
{
    if (QFile::exists(QStringLiteral("/usr/sbin/smbd"))
            || QFile::exists(QStringLiteral("/usr/local/sbin/smbd"))) {
        return true;
    }

    //qDebug() << "Samba is not installed!";

    return false;
}

SambaUserSharePlugin::SambaUserSharePlugin(QObject *parent, const QList<QVariant> &args)
    : KPropertiesDialogPlugin(qobject_cast<KPropertiesDialog *>(parent))
    , m_url(properties->item().mostLocalUrl().toLocalFile())
    , shareData()
{
    Q_UNUSED(args);

    if (m_url.isEmpty()) {
        return;
    }

    QFileInfo pathInfo(m_url);
    if (!pathInfo.permission(QFile::ReadUser | QFile::WriteUser)) {
        return;
    }

    QFrame *vbox = new QFrame();
    properties->addPage(vbox, i18n("&Share"));
    properties->setFileSharingPage(vbox);
    QVBoxLayout *vLayoutMaster = new QVBoxLayout(vbox);

    m_failedSambaWidgets = new QWidget(vbox);
    vLayoutMaster->addWidget(m_failedSambaWidgets);
    QVBoxLayout *vFailedLayout = new QVBoxLayout(m_failedSambaWidgets);
    vFailedLayout->setAlignment(Qt::AlignJustify);
    vFailedLayout->setMargin(0);
    vFailedLayout->addWidget(new QLabel(i18n("The Samba package failed to install."), m_failedSambaWidgets));
    vFailedLayout->addStretch();
    m_failedSambaWidgets->hide();

    m_installSambaWidgets = new QWidget(vbox);
    vLayoutMaster->addWidget(m_installSambaWidgets);
    QVBoxLayout *vLayout = new QVBoxLayout(m_installSambaWidgets);
    vLayout->setAlignment(Qt::AlignJustify);
    vLayout->setMargin(0);

    vLayout->addWidget(new QLabel(i18n("Samba is not installed on your system."), m_installSambaWidgets));

#ifdef SAMBA_INSTALL
    m_installSambaButton = new QPushButton(i18n("Install Samba..."), m_installSambaWidgets);
    m_installSambaButton->setDefault(false);
    vLayout->addWidget(m_installSambaButton);
    connect(m_installSambaButton, SIGNAL(clicked()), SLOT(installSamba()));
    m_installProgress = new QProgressBar();
    vLayout->addWidget(m_installProgress);
    m_installProgress->hide();
#endif

    // align items on top
    vLayout->addStretch();
    m_shareWidgets = new QWidget(vbox);
    vLayoutMaster->addWidget(m_shareWidgets);
    propertiesUi.setupUi(m_shareWidgets);

    QList<KSambaShareData> shareList = KSambaShare::instance()->getSharesByPath(m_url);

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
    connect(propertiesUi.sambaAllowGuestChk, &QCheckBox::toggled,
            this, [=] (bool checked) {
                propertiesUi.tableView->setEnabled(checked && propertiesUi.sambaChk->isChecked());
                setDirty();
            });
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(changed()));

    for (int i = 0; i < model->rowCount(); ++i) {
        propertiesUi.tableView->openPersistentEditor(model->index(i, 1, QModelIndex()));
    }
    if (!isSambaInstalled()) {
        m_installSambaWidgets->show();
        m_shareWidgets->hide();
    } else {
        m_installSambaWidgets->hide();
        m_shareWidgets->show();
    }
}

SambaUserSharePlugin::~SambaUserSharePlugin()
{
}

#ifdef SAMBA_INSTALL
void SambaUserSharePlugin::installSamba()
{
    QString package = QStringLiteral(SAMBA_PACKAGE_NAME);
    PackageKit::Transaction *transaction = PackageKit::Daemon::resolve(package,
                                                   PackageKit::Transaction::FilterArch);
    connect(transaction,
            SIGNAL(package(PackageKit::Transaction::Info,QString,QString)),
            SLOT(packageInstall(PackageKit::Transaction::Info,QString,QString)));
    m_installProgress->setMaximum(0);
    m_installProgress->setMinimum(0);
    m_installProgress->show();
    m_installSambaButton->hide();
}

void SambaUserSharePlugin::packageInstall(PackageKit::Transaction::Info info,
                                          const QString &packageId,
                                          const QString &summary)
{
    Q_UNUSED(info);
    Q_UNUSED(summary);
    PackageKit::Transaction *installTransaction = PackageKit::Daemon::installPackage(packageId);
    connect(installTransaction,
            SIGNAL(finished(PackageKit::Transaction::Exit, uint)),
            SLOT(packageFinished(PackageKit::Transaction::Exit, uint)));
}

void SambaUserSharePlugin::packageFinished(PackageKit::Transaction::Exit status, uint runtime)
{
    Q_UNUSED(runtime);
    if (status == PackageKit::Transaction::ExitSuccess) {
        m_installSambaWidgets->hide();
        m_failedSambaWidgets->hide();
        m_shareWidgets->show();
    } else {
        m_shareWidgets->hide();
        m_installSambaWidgets->hide();
        m_failedSambaWidgets->show();
    }
}
#endif // SAMBA_INSTALL

void SambaUserSharePlugin::setupModel()
{
    model = new UserPermissionModel(shareData, this);
}

void SambaUserSharePlugin::setupViews()
{
    propertiesUi.tableView->setModel(model);
    propertiesUi.tableView->setSelectionMode(QAbstractItemView::NoSelection);
    propertiesUi.tableView->setItemDelegate(new UserPermissionDelegate(this));
    propertiesUi.tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

void SambaUserSharePlugin::load()
{
    bool guestAllowed = false;
    bool sambaShared = KSambaShare::instance()->isDirectoryShared(m_url);

    propertiesUi.sambaChk->setChecked(sambaShared);
    toggleShareStatus(sambaShared);
    if (sambaShared) {
        guestAllowed = (bool) shareData.guestPermission();
    }
    propertiesUi.sambaAllowGuestChk->setChecked(guestAllowed);
    propertiesUi.tableView->setEnabled(propertiesUi.sambaChk->isChecked() &&
                                       propertiesUi.sambaAllowGuestChk->isChecked());

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

        shareData.setPath(m_url);

        KSambaShareData::GuestPermission guestOk(shareData.guestPermission());

        guestOk = (propertiesUi.sambaAllowGuestChk->isChecked() == false)
                  ? KSambaShareData::GuestsNotAllowed : KSambaShareData::GuestsAllowed;

        shareData.setGuestPermission(guestOk);

        result = shareData.save();
    } else if (KSambaShare::instance()->isDirectoryShared(m_url)) {
        result = shareData.remove();
    }
}

void SambaUserSharePlugin::toggleShareStatus(bool checked)
{
    propertiesUi.textLabel1->setEnabled(checked);
    propertiesUi.sambaNameEdit->setEnabled(checked);
    propertiesUi.sambaAllowGuestChk->setEnabled(checked);
    propertiesUi.tableView->setEnabled(checked && propertiesUi.sambaAllowGuestChk->isChecked());
    if (checked && propertiesUi.sambaNameEdit->text().isEmpty()) {
        propertiesUi.sambaNameEdit->setText(getNewShareName());
    } else {
        propertiesUi.sambaNameEdit->setText(QString());
    }
}

void SambaUserSharePlugin::checkShareName(const QString &name)
{
    // Don't ever disable the OK button when the user is trying to remove a share
    if (!propertiesUi.sambaChk->isChecked()) {
        return;
    }

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
        properties->button(QDialogButtonBox::Ok)->setEnabled(false);
        propertiesUi.sambaNameEdit->setFocus();
        return;
    }

    if (!properties->button(QDialogButtonBox::Ok)->isEnabled()) {
        properties->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

QString SambaUserSharePlugin::getNewShareName()
{
    QString shareName = QUrl(m_url).fileName();

    if (!propertiesUi.sambaNameEdit->text().isEmpty()) {
        shareName = propertiesUi.sambaNameEdit->text();
    }

    // Windows could have problems with longer names
    shareName = shareName.left(12);

    return shareName;
}

#include "sambausershareplugin.moc"
