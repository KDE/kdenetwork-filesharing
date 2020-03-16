// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#include "plugin.h"

#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QVBoxLayout>

#include <KDeclarative/KDeclarative>
#include <KIO/SpecialJob>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KPluginFactory>

#include "aceobject.h"
#include "debug.h"
#include "model.h"

K_PLUGIN_CLASS_WITH_JSON(SambaACL, "samba-acl.json")

constexpr int getACEMagic = 0xAC;
constexpr int setACEMagic = 0xACD;

class Context : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    Q_PROPERTY(Model *aceModel MEMBER m_aceModel CONSTANT)
    Model *m_aceModel = new Model(this);

    Q_PROPERTY(QList<QVariantMap> types READ types CONSTANT)
    [[nodiscard]] Q_INVOKABLE QList<QVariantMap> types()
    {
        static QList<QVariantMap> ret;
        if (!ret.isEmpty()) {
            return ret;
        }
        const auto enumerable = QMetaEnum::fromType<ACEObject::Type>();
        for (int i = 0; i < enumerable.keyCount(); ++i) {
            const int value = enumerable.value(i);
            QVariantMap map;
            map[QStringLiteral("text")] = typeToString(static_cast<ACEObject::Type>(value));
            map[QStringLiteral("value")] = value;
            ret << map;

        }
        return ret;
    }

    [[nodiscard]] static QString typeToString(ACEObject::Type type)
    {
        switch (type) {
        case ACEObject::Type::Deny:
            return i18nc("@option:radio an entry denying permissions", "Deny");
        case ACEObject::Type::Allow:
            return i18nc("@option:radio an entry allowing permissions", "Allow");
        }
        // We only support deny and allow for now (and samba does to apparently)
        Q_UNREACHABLE();
        return i18nc("@option:radio an unknown permission entry type (doesn't really happen)", "Unknown");
    }

    [[nodiscard]] static QString inheritanceToString(ACEObject::Inheritance inheritance)
    {
        switch (inheritance) {
        case ACEObject::Inheritance::None:
            return i18nc("@option:radio permission applicability type", "This folder only");
        case ACEObject::Inheritance::FolderSubfoldersFiles:
            return i18nc("@option:radio permission applicability type", "This folder, subfolders and files");
        case ACEObject::Inheritance::FolderSubfolders:
            return i18nc("@option:radio permission applicability type", "This folder and subfolders");
        case ACEObject::Inheritance::FolderFiles:
            return i18nc("@option:radio permission applicability type", "This folder and files");
        case ACEObject::Inheritance::SubfoldersFiles:
            return i18nc("@option:radio permission applicability type", "Subfolders and files only");
        case ACEObject::Inheritance::Subfolders:
            return i18nc("@option:radio permission applicability type", "Subfolders only");
        case ACEObject::Inheritance::Files:
            return i18nc("@option:radio permission applicability type", "Files only");
        }
        Q_UNREACHABLE();
        return i18nc("@option:radio permission applicability type (doesn't really happen)", "Unknown");
    }

    Q_PROPERTY(QList<QVariantMap> inheritances READ inheritances CONSTANT)
    [[nodiscard]] Q_INVOKABLE QList<QVariantMap> inheritances()
    {
        static QList<QVariantMap> ret;
        if (!ret.isEmpty()) {
            return ret;
        }
        const auto enumerable = QMetaEnum::fromType<ACEObject::Inheritance>();
        for (int i = 0; i < enumerable.keyCount(); ++i) {
            const int value = enumerable.value(i);
            QVariantMap map;
            map[QStringLiteral("text")] = inheritanceToString(static_cast<ACEObject::Inheritance>(value));
            map[QStringLiteral("value")] = value;
            ret << map;

        }
        return ret;
    }

    Q_PROPERTY(QString owner MEMBER m_owner NOTIFY ownerChanged);
    Q_SIGNAL void ownerChanged();
    QString m_owner;

    Q_PROPERTY(QString group MEMBER m_group NOTIFY groupChanged);
    Q_SIGNAL void groupChanged();
    QString m_group;
};

static Context &context()
{
    static Context s_context;
    return s_context;
}

    // TODO maybe introduce a unix mode if Unix Group\ exists. only a quarter of the ace mask has meaning because it translates to rwx

    /*
     * POSIX ACL
     * rwx => (ALL ACTRL mask bits set) :: 00000000000111110000000111111111
     * rw- => 00000000000100100000000010101001 :: ACTRL_DS_CREATE_CHILD | ACTRL_DS_SELF | ACTRL_DS_WRITE_PROP | ACTRL_DS_LIST_OBJECT | ACTRL_FILE_READ | ACTRL_FILE_READ_PROP | ACTRL_FILE_EXECUTE | ACTRL_FILE_READ_ATTRIB | ACTRL_DIR_LIST | ACTRL_DIR_TRAVERSE
     * r-- => 00000000000100100000000010001001 :: ACTRL_DS_CREATE_CHILD | ACTRL_DS_SELF | ACTRL_DS_LIST_OBJECT | ACTRL_FILE_READ | ACTRL_FILE_READ_PROP | ACTRL_FILE_READ_ATTRIB | ACTRL_DIR_LIST
     *
     * POSIX ACL default:* entries are mapped in the flags if applicable (i.e. INHERIT_ONLY_ACE | CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE)
     */

SambaACL::SambaACL(QObject *parent, const QList<QVariant> &args)
    : KPropertiesDialogPlugin(qobject_cast<KPropertiesDialog *>(parent))
    , m_url(properties->url())
    , m_page(new QWidget(qobject_cast<KPropertiesDialog *>(parent)))
{
    Q_UNUSED(args);

    auto parts = m_url.path().split(QLatin1Char('/'));
    parts.removeAll(QLatin1String());
    if (!m_url.isValid() || parts.isEmpty()) {
        return; // neither root nor host have permissions, shares may
    }

    qmlRegisterType<Model>("org.kde.filesharing.samba.acl", 1, 0, "ACEModel");

    auto engine = new QQmlApplicationEngine(this);
    m_page->setAttribute(Qt::WA_TranslucentBackground);
    auto widget = new QQuickWidget(engine, m_page.get());

    KDeclarative::KDeclarative::setupEngine(widget->engine());
    auto i18nContext = new KLocalizedContext(widget->engine());
    i18nContext->setTranslationDomain(QStringLiteral(TRANSLATION_DOMAIN));
    widget->engine()->rootContext()->setContextObject(i18nContext);

    widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    widget->setFocusPolicy(Qt::StrongFocus);
    widget->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    widget->quickWindow()->setColor(Qt::transparent);
    auto layout = new QVBoxLayout(m_page.get());
    layout->addWidget(widget);

    qmlRegisterSingletonInstance<Context>("org.kde.filesharing.samba.acl", 1, 0, "Context", &context());
    widget->rootContext()->setContextProperty(QStringLiteral("plugin"), this);

    const QUrl url(QStringLiteral("qrc:/org.kde.filesharing.samba.acl/qml/main.qml"));

    QObject::connect(
        engine,
        &QQmlApplicationEngine::objectCreated,
        this,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                qFatal("qml error");
            }
        },
        Qt::QueuedConnection);

    widget->setSource(url);

    if (!widget->rootObject()) {
        qFatal("error");
    }

    (void)properties->addPage(m_page.get(), i18nc("@title:tab", "Remote Permissions"));
    if (auto parentWidget = qobject_cast<QWidget *>(m_page->parent()); parentWidget && parentWidget->layout()) {
        // Force our encompassing layout to have no margins, our QML Controls have some already!
        parentWidget->layout()->setContentsMargins(0,0,0,0);
    }

    // TODO make this more discriminatory
    setDirty(true);

    refresh();
}

void SambaACL::applyChanges()
{
    const auto acl = context().m_aceModel->acl();
    for (const auto &ace : acl) {
        if (ace->flags & INHERITED_ACE) { // cannot possibly be modified
            continue;
        }
        if (ace->originalXattr == ace->toSMBXattr()) { // unchanged
            continue;
        }

        qWarning() << "APPLYING CHANGES for!" << ace->sid;
        QByteArray packedArgs;
        QDataStream stream(&packedArgs, QIODevice::WriteOnly);
        stream << setACEMagic << m_url << ace->sid << ace->toSMBXattr();

        // TODO could start multiple setters maybe then wait for all of them
        auto job = KIO::special(m_url, packedArgs);
        job->exec();
    }
}

void SambaACL::refresh()
{
    QByteArray packedArgs;
    QDataStream stream(&packedArgs, QIODevice::WriteOnly);
    stream << getACEMagic << m_url;

    auto job = KIO::special(m_url, packedArgs);
    connect(job, &KJob::finished, this, [this, job] {
        const QString aclString = job->metaData().value(QStringLiteral("ACL"));
        context().setProperty("owner", job->metaData().value(QStringLiteral("OWNER")));
        context().setProperty("group", job->metaData().value(QStringLiteral("GROUP")));

        const auto aceStrings = aclString.split(QLatin1Char(','));
        QList<std::shared_ptr<ACE>> acl;
        QRegularExpression r(QStringLiteral("(?<SID>.+):(?<TYPE>\\d+)/(?<FLAGS>\\d+)/(?<MASK>0[xX][0-9a-fA-F]+)"));
        for (const auto &aceString : aceStrings) {
            const auto match = r.match(aceString);
            qDebug() << match << aceString;
            if (!match.isValid() || !match.hasMatch()) {
                continue;
            }

            std::shared_ptr<ACE> ace(new ACE{match.captured(QStringLiteral("SID")),
                                             (uint8_t)match.captured(QStringLiteral("TYPE")).toUShort(),
                                             (uint8_t)match.captured(QStringLiteral("FLAGS")).toUShort(),
                                             match.captured(QStringLiteral("MASK")).toUInt(nullptr, 16)});

            if (qEnvironmentVariableIntValue("KIO_SMB_ACL_DEBUG") > 1) {
                printACE(*ace);
            }
            acl << ace;
        }

        context().m_aceModel->resetData(acl);

        m_ready = true;
        Q_EMIT readyChanged();
    });
    job->start();
}

#include "plugin.moc"
