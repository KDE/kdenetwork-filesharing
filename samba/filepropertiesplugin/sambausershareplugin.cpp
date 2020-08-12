/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
  Copyright (c) 2011 Rodrigo Belem <rclbelem@gmail.com>
  Copyright (c) 2015-2020 Harald Sitter <sitter@kde.org>
  Copyright (c) 2019 Nate Graham <nate@kde.org>

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

#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QFrame>
#include <QIcon>
#include <QPushButton>
#include <QStandardPaths>
#include <QStringList>
#include <QDebug>

#include <KMessageBox>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KSambaShare>
#include <KSambaShareData>
#include <ktoolinvocation.h>

#include "sambausershareplugin.h"
#include "model.h"
#include "delegate.h"

K_PLUGIN_FACTORY(SambaUserSharePluginFactory, registerPlugin<SambaUserSharePlugin>();)

// copied from kio/src/core/ksambashare.cpp, KSambaSharePrivate::isSambaInstalled()
static bool isSambaInstalled()
{
    return QFile::exists(QStringLiteral("/usr/sbin/smbd"))
           || QFile::exists(QStringLiteral("/usr/local/sbin/smbd"));
}

SambaUserSharePlugin::SambaUserSharePlugin(QObject *parent, const QList<QVariant> &args)
    : KPropertiesDialogPlugin(qobject_cast<KPropertiesDialog *>(parent))
    , m_url(properties->item().mostLocalUrl().toLocalFile())
    , shareData()
{
    Q_UNUSED(args)

    if (m_url.isEmpty()) {
        return;
    }

    const QFileInfo pathInfo(m_url);
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
    vFailedLayout->setContentsMargins(0, 0, 0, 0);
    vFailedLayout->addWidget(new QLabel(i18n("The Samba package failed to install."), m_failedSambaWidgets));
    vFailedLayout->addStretch();
    m_failedSambaWidgets->hide();

    m_installSambaWidgets = new QWidget(vbox);
    vLayoutMaster->addWidget(m_installSambaWidgets);
    QVBoxLayout *vLayout = new QVBoxLayout(m_installSambaWidgets);
    vLayout->setAlignment(Qt::AlignJustify);
    vLayout->setContentsMargins(0, 0, 0, 0);

    m_sambaStatusMessage = new QLabel(i18n("Samba must be installed before folders can be shared."));
    m_sambaStatusMessage->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(m_sambaStatusMessage);

#ifdef SAMBA_INSTALL
    m_justInstalledSambaWidgets = new QWidget(vbox);
    vLayoutMaster->addWidget(m_justInstalledSambaWidgets);
    QVBoxLayout *vJustInstalledLayout = new QVBoxLayout(m_justInstalledSambaWidgets);
    vJustInstalledLayout->setAlignment(Qt::AlignJustify);
    vJustInstalledLayout->addWidget(new QLabel(i18n("Restart the computer to complete the installation."), m_justInstalledSambaWidgets));
    m_restartButton = new QPushButton(i18n("Restart"), m_justInstalledSambaWidgets);
    m_restartButton->setIcon(QIcon::fromTheme(QStringLiteral("system-reboot")));
    connect(m_restartButton, &QPushButton::clicked,
            this, &SambaUserSharePlugin::reboot);
    vJustInstalledLayout->addWidget(m_restartButton);
    vJustInstalledLayout->addStretch();
    m_restartButton->setDefault(false);
    m_justInstalledSambaWidgets->hide();

    m_installSambaButton = new QPushButton(i18n("Install Samba"), m_installSambaWidgets);
    m_installSambaButton->setDefault(false);
    vLayout->addWidget(m_installSambaButton);
    connect(m_installSambaButton, &QPushButton::clicked,
            this, &SambaUserSharePlugin::installSamba);
    m_installProgress = new QProgressBar();
    vLayout->addWidget(m_installProgress);
    m_installProgress->hide();
#endif

    // align items on top
    vLayout->addStretch();
    m_shareWidgets = new QWidget(vbox);
    vLayoutMaster->addWidget(m_shareWidgets);
    propertiesUi.setupUi(m_shareWidgets);

    const QList<KSambaShareData> shareList = KSambaShare::instance()->getSharesByPath(m_url);

    if (!shareList.isEmpty()) {
        shareData = shareList.at(0); // FIXME: using just the first in the list for a while
    }

    setupModel();
    setupViews();
    load();

    connect(propertiesUi.sambaChk, &QCheckBox::toggled,
            this, &SambaUserSharePlugin::toggleShareStatus);
    connect(propertiesUi.sambaNameEdit, &QLineEdit::textChanged,
            this, &SambaUserSharePlugin::checkShareName);
    connect(propertiesUi.sambaAllowGuestChk, &QCheckBox::toggled,
            this, [=] (bool checked) { setDirty(); });
    connect(model, &UserPermissionModel::dataChanged,
            this, [=] { setDirty(); });
    connect(propertiesUi.sambaStatusMonitorButton, &QPushButton::clicked,
            this, [] {
                KToolInvocation::kdeinitExec(QStringLiteral("kcmshell5"), {QStringLiteral("smbstatus")});
            });

    for (int i = 0; i < model->rowCount(); ++i) {
        propertiesUi.tableView->openPersistentEditor(model->index(i, UserPermissionModel::ColumnAccess, QModelIndex()));
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
    const QString package = QStringLiteral(SAMBA_PACKAGE_NAME);
    QStringList distroSambaPackages = package.split(QLatin1Char(','));

    PackageKit::Transaction *transaction = PackageKit::Daemon::resolve(distroSambaPackages, PackageKit::Transaction::FilterArch);

    QSharedPointer<QStringList> pkgids(new QStringList);

    connect(transaction, &PackageKit::Transaction::package,
            this, [pkgids](PackageKit::Transaction::Info /*info*/, const QString &packageId,
                           const QString & /*summary*/) { pkgids->append(packageId); });

    connect(transaction, &PackageKit::Transaction::finished,
            this, [this, pkgids] (PackageKit::Transaction::Exit exit) {
                if (exit != PackageKit::Transaction::ExitSuccess) { return; }
                auto installTransaction = PackageKit::Daemon::installPackages(*pkgids);
                connect(installTransaction, &PackageKit::Transaction::finished,
                        this, &SambaUserSharePlugin::packageFinished);
            }
    );

    m_sambaStatusMessage->setText(i18n("Installing Samba..."));
    m_installProgress->setMaximum(0);
    m_installProgress->setMinimum(0);
    m_installProgress->show();
    m_installSambaButton->hide();
}

void SambaUserSharePlugin::packageFinished(PackageKit::Transaction::Exit status, uint runtime)
{
    Q_UNUSED(runtime)
    if (status == PackageKit::Transaction::ExitSuccess) {
        m_installSambaWidgets->hide();
        m_failedSambaWidgets->hide();
        m_shareWidgets->hide();
        m_justInstalledSambaWidgets->show();
    } else {
        m_shareWidgets->hide();
        m_installSambaWidgets->hide();
        m_failedSambaWidgets->show();
    }
}

void SambaUserSharePlugin::reboot()
{
    QDBusInterface interface(QStringLiteral("org.kde.ksmserver"), QStringLiteral("/KSMServer"),
                             QStringLiteral("org.kde.KSMServerInterface"), QDBusConnection::sessionBus());
    interface.asyncCall(QStringLiteral("logout"), 0, 1, 2); // Options: do not ask again | reboot | force
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
    propertiesUi.tableView->horizontalHeader()->setSectionResizeMode(UserPermissionModel::ColumnAccess,
                                                                     QHeaderView::Stretch);
}

void SambaUserSharePlugin::load()
{
    bool guestAllowed = false;
    const bool sambaShared = KSambaShare::instance()->isDirectoryShared(m_url);

    propertiesUi.sambaChk->setChecked(sambaShared);
    toggleShareStatus(sambaShared);
    if (sambaShared) {
        guestAllowed = (bool) shareData.guestPermission();
    }
    propertiesUi.sambaAllowGuestChk->setChecked(guestAllowed);
    propertiesUi.tableView->setEnabled(propertiesUi.sambaChk->isChecked());

    propertiesUi.sambaNameEdit->setText(shareData.name());
}

void SambaUserSharePlugin::applyChanges()
{
    if (propertiesUi.sambaChk->isChecked()) {
        if (shareData.setAcl(model->getAcl()) != KSambaShareData::UserShareAclOk) {
            return;
        }

        shareData.setName(propertiesUi.sambaNameEdit->text());

        shareData.setPath(m_url);

        KSambaShareData::GuestPermission guestOk(shareData.guestPermission());

        guestOk = !propertiesUi.sambaAllowGuestChk->isChecked()
                  ? KSambaShareData::GuestsNotAllowed : KSambaShareData::GuestsAllowed;

        shareData.setGuestPermission(guestOk);

        reportAdd(shareData.save());
    } else if (KSambaShare::instance()->isDirectoryShared(m_url)) {
        reportRemove(shareData.remove());
    }
}

static QString errorToString(KSambaShareData::UserShareError error)
{
    // KSambaShare is a right mess. Every function with an error returns the same enum but can only return a subset of
    // possible values. Even so, because it returns the enum we had best mapped all values to semi-suitable string
    // representations even when those are utter garbage when they require specific (e.g. an invalid ACL) that
    // we do not have here.
    switch (error) {
    case KSambaShareData::UserShareNameOk: Q_FALLTHROUGH();
    case KSambaShareData::UserSharePathOk: Q_FALLTHROUGH();
    case KSambaShareData::UserShareAclOk: Q_FALLTHROUGH();
    case KSambaShareData::UserShareCommentOk: Q_FALLTHROUGH();
    case KSambaShareData::UserShareGuestsOk: Q_FALLTHROUGH();
    case KSambaShareData::UserShareOk:
        // Technically anything but UserShareOk cannot happen, but best handle everything regardless.
        return QString();
    case KSambaShareData::UserShareExceedMaxShares:
        return i18nc("@info detailed error messsage",
                     "You have exhausted the maximum amount of shared directories you may have active at the same time.");
    case KSambaShareData::UserShareNameInvalid:
        return i18nc("@info detailed error messsage", "The share name is invalid.");
    case KSambaShareData::UserShareNameInUse:
        return i18nc("@info detailed error messsage", "The share name is already in use for a different directory.");
    case KSambaShareData::UserSharePathInvalid:
        return i18nc("@info detailed error messsage", "The path is invalid.");
    case KSambaShareData::UserSharePathNotExists:
        return i18nc("@info detailed error messsage", "The path does not exist.");
    case KSambaShareData::UserSharePathNotDirectory:
        return i18nc("@info detailed error messsage", "The path is not a directory.");
    case KSambaShareData::UserSharePathNotAbsolute:
        return i18nc("@info detailed error messsage", "The path is relative.");
    case KSambaShareData::UserSharePathNotAllowed:
        return i18nc("@info detailed error messsage", "This path may not be shared.");
    case KSambaShareData::UserShareAclInvalid:
        return i18nc("@info detailed error messsage", "The access rule is invalid.");
    case KSambaShareData::UserShareAclUserNotValid:
        return i18nc("@info detailed error messsage", "An access rule's user is not valid.");
    case KSambaShareData::UserShareGuestsInvalid:
        return i18nc("@info detailed error messsage", "The 'Guest' access rule is invalid.");
    case KSambaShareData::UserShareGuestsNotAllowed:
        return i18nc("@info detailed error messsage", "Enabling guest access is not allowed.");
    case KSambaShareData::UserShareSystemError:
        return KSambaShare::instance()->lastSystemErrorString().simplified();
    }
    Q_UNREACHABLE();
    return QString();
}

void SambaUserSharePlugin::reportAdd(KSambaShareData::UserShareError error)
{
    if (error == KSambaShareData::UserShareOk) {
        return;
    }

    QString errorMessage = errorToString(error);
    if (error == KSambaShareData::UserShareSystemError) {
        // System errors are (untranslated) CLI output. Give them localized context.
        errorMessage = xi18nc("@info error in the underlying binaries. %1 is CLI output",
                              "<para>An error occurred while trying to share the directory."
                              " The share has not been created.</para>"
                              "<para>Samba internals report:</para><message>%1</message>",
                              errorMessage);
    }
    KMessageBox::error(qobject_cast<QWidget *>(parent()),
                       errorMessage,
                       i18nc("@info/title", "Failed to Create Network Share"));
}

void SambaUserSharePlugin::reportRemove(KSambaShareData::UserShareError error)
{
    if (error == KSambaShareData::UserShareOk) {
        return;
    }

    QString errorMessage = errorToString(error);
    if (error == KSambaShareData::UserShareSystemError) {
        // System errors are (untranslated) CLI output. Give them localized context.
        errorMessage = xi18nc("@info error in the underlying binaries. %1 is CLI output",
                              "<para>An error occurred while trying to un-share the directory."
                              " The share has not been removed.</para>"
                              "<para>Samba internals report:</para><message>%1</message>",
                              errorMessage);
    }
    KMessageBox::error(qobject_cast<QWidget *>(parent()),
                       errorMessage,
                       i18nc("@info/title", "Failed to Remove Network Share"));
}

void SambaUserSharePlugin::toggleShareStatus(bool checked)
{
    propertiesUi.textLabel1->setEnabled(checked);
    propertiesUi.sambaNameEdit->setEnabled(checked);
    propertiesUi.sambaAllowGuestChk->setEnabled(checked);
    propertiesUi.tableView->setEnabled(checked);
    if (checked && propertiesUi.sambaNameEdit->text().isEmpty()) {
        propertiesUi.sambaNameEdit->setText(getNewShareName());
    } else {
        propertiesUi.sambaNameEdit->setText(QString());
    }
    setDirty();
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
        setDirty();
    }
}

QString SambaUserSharePlugin::getNewShareName() const
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
