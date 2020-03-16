// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#pragma once

#include <cstdint>

#include <QString>

// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-ace_header
struct ACE { // roughly _ACE_HEADER
    ACE(const QString &sid_, uint8_t type_, uint8_t flags_, uint32_t mask_)
        : sid(sid_)
        , type(type_)
        , flags(flags_)
        , mask(mask_)
        , originalXattr(toSMBXattr())
    {
    }

    const QString sid;
    uint8_t type; // BYTE
    uint8_t flags; // BYTE
    uint32_t mask; // DWORD
    const QString originalXattr; // toSMBXattr at construction time

    QString toSMBXattr() const
    {
        // NB: the mask should be 0xHEX to be the same as the input format.
        //   libsmbc doesn't correctly parse 0xHEX masks though and ends up
        //   setting 0x0. Specifically it calls sscanf with %u even when it
        //   explicitly verified the input is 0x and would require %x to
        //   correctly parse it.
        return QStringLiteral("%1/%2/%3").arg(type).arg(flags).arg(mask);
    }
};
