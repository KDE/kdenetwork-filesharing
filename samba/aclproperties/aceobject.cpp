// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#include "aceobject.h"

#include <QDebug>
#include <QMetaEnum>

#include <KLocalizedString>

static int flagsToInheritance(uint8_t flags)
{
    const auto inheritanceEnum = QMetaEnum::fromType<ACEObject::Inheritance>();
    for (int i = 0; i < inheritanceEnum.keyCount(); ++i) {
        const int value = inheritanceEnum.value(i);
        if ((flags & value) == value) {
            return value;
        }
    }
    return static_cast<int>(ACEObject::Inheritance::None);
}

ACEObject::ACEObject(const std::shared_ptr<ACE> &ace, QObject *parent)
    : QObject(parent)
    , m_ace(ace)
    , m_inherited(ace->flags & INHERITED_ACE)
{
}

unsigned int ACEObject::type() const
{
    return m_ace->type;
}

void ACEObject::setType(unsigned int type)
{
    m_ace->type = type;
    Q_ASSERT(m_ace->type == type);
    Q_EMIT typeChanged();
}

int ACEObject::inheritance() const
{
    return flagsToInheritance(m_ace->flags);
}

void ACEObject::setInheritance(int intInheritance)
{
    m_ace->flags = (m_ace->flags ^ inheritance()) | intInheritance;
    Q_EMIT inheritanceChanged();
}

bool ACEObject::noPropagate() const
{
    return m_ace->flags & NO_PROPAGATE_INHERIT_ACE;
}

void ACEObject::setNoPropagate(bool noPropagate)
{
    m_ace->flags = noPropagate ? (m_ace->flags | NO_PROPAGATE_INHERIT_ACE) : (m_ace->flags ^ NO_PROPAGATE_INHERIT_ACE);
    Q_EMIT noPropagateChanged();
}
