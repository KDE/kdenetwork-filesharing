// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#pragma once

#include <memory>

#include <QObject>

#include "acccrtl.h"
#include "ace.h"
#include "debug.h"
#include "winnt.h"

class ACEObject : public QObject
{
    Q_OBJECT
public:
    enum class Type {
        Deny = ACCESS_DENIED_ACE_TYPE,
        Allow = ACCESS_ALLOWED_ACE_TYPE,
        // Audit = SYSTEM_AUDIT_ACE_TYPE,
        // Alarm = SYSTEM_ALARM_ACE_TYPE,
        // MandatoryLabel = SYSTEM_MANDATORY_LABEL_ACE_TYPE,
    };
    Q_ENUM(Type)

    enum class Inheritance {
        // NB: order by amount of flags, a match for a|b|c should outscore a|b
        SubfoldersFiles = INHERIT_ONLY_ACE | CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE,
        Subfolders = INHERIT_ONLY_ACE | CONTAINER_INHERIT_ACE,
        Files = INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE,
        FolderSubfoldersFiles = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE,
        FolderSubfolders = CONTAINER_INHERIT_ACE,
        FolderFiles = OBJECT_INHERIT_ACE,
        None = 0x0,
    };
    Q_ENUM(Inheritance)

    explicit ACEObject(const std::shared_ptr<ACE> &ace, QObject *parent = nullptr);

    std::shared_ptr<ACE> m_ace;

    Q_PROPERTY(bool inherited MEMBER m_inherited CONSTANT)
    const bool m_inherited;

    Q_PROPERTY(unsigned int type READ type WRITE setType NOTIFY typeChanged)
    unsigned int type() const;
    void setType(unsigned int type);
    Q_SIGNAL void typeChanged();

    Q_PROPERTY(int inheritance READ inheritance WRITE setInheritance NOTIFY inheritanceChanged)
    int inheritance() const;
    void setInheritance(int intInheritance);
    Q_SIGNAL void inheritanceChanged();

    Q_PROPERTY(bool noPropagate READ noPropagate WRITE setNoPropagate NOTIFY noPropagateChanged)
    bool noPropagate() const;
    void setNoPropagate(bool noPropagate);
    Q_SIGNAL void noPropagateChanged();

#define MASK_PROPERTY(name, value)                                                                                                                             \
    Q_PROPERTY(bool name READ name WRITE set_##name NOTIFY name##Changed)                                                                                      \
                                                                                                                                                               \
public:                                                                                                                                                        \
    Q_SIGNAL void name##Changed();                                                                                                                             \
    [[nodiscard]] bool name()                                                                                                                                  \
    {                                                                                                                                                          \
        return (m_ace->mask & (value));                                                                                                                        \
    }                                                                                                                                                          \
    void set_##name(bool check)                                                                                                                                \
    {                                                                                                                                                          \
        fprintf_binary(stderr, m_ace->mask);                                                                                                                   \
        m_ace->mask = check ? (m_ace->mask | (value)) : (m_ace->mask ^ (value));                                                                               \
        fprintf_binary(stderr, m_ace->mask);                                                                                                                   \
        Q_EMIT name##Changed();                                                                                                                                \
    }

    MASK_PROPERTY(takeOwnership, WRITE_OWNER)
    MASK_PROPERTY(changePermissions, WRITE_DAC)
    MASK_PROPERTY(readPermissions, READ_CONTROL)
    MASK_PROPERTY(canDelete, DELETE)
    MASK_PROPERTY(canDeleteData, ACTRL_DIR_DELETE_CHILD) // not a thing for files
    MASK_PROPERTY(writeExtendedAttributes, ACTRL_FILE_WRITE_PROP)
    MASK_PROPERTY(writeAttributes, ACTRL_FILE_WRITE_ATTRIB)
    MASK_PROPERTY(appendData, ACTRL_FILE_APPEND) // aka ACTRL_DIR_CREATE_CHILD
    MASK_PROPERTY(writeData, ACTRL_FILE_WRITE)
    MASK_PROPERTY(readExtendedAttributes, ACTRL_FILE_READ_PROP)
    MASK_PROPERTY(readAttributes, ACTRL_FILE_READ_ATTRIB)
    MASK_PROPERTY(readData, ACTRL_FILE_READ) // aka ACTRL_DIR_LIS
    MASK_PROPERTY(execute, ACTRL_FILE_EXECUTE) // aka ACTRL_DIR_TRAVERSE

#undef MASK_PROPERTY
};
