// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#include "debug.h"

#include <QDebug>

#include "acccrtl.h"
#include "ace.h"
#include "winnt.h"

// Various helpful resources:
// https://docs.microsoft.com/en-us/windows/win32/secauthz/access-control-entries
// https://docs.microsoft.com/en-us/windows/win32/secauthz/access-rights-and-access-masks
// https://docs.microsoft.com/en-us/windows/win32/secauthz/directory-services-access-rights
// https://docs.microsoft.com/en-us/windows/win32/ad/example-code-for-setting-an-ace-on-a-directory-object
// https://docs.microsoft.com/en-us/windows/win32/ad/control-access-rights
// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-dtyp/628ebb1d-c509-4ea0-a10f-77ef97ca4586
// https://docs.microsoft.com/en-us/archive/blogs/openspecification/about-the-access_mask-structure
// https://cpp.hotexamples.com/examples/-/-/GetAce/cpp-getace-function-examples.html
// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-access_allowed_ace
// https://docs.microsoft.com/en-us/windows/win32/secauthz/authorization-data-types

void fprintf_binary(FILE *o, uint32_t v, bool newline)
{
    uint32_t mask = 1 << ((sizeof(uint32_t) << 3) - 1);
    while (mask) {
        (void)fprintf(o, "%d", (v & mask ? 1 : 0));
        mask >>= 1;
    }
    if (newline) {
        (void)fprintf(o, "\n");
    }
}

void printType(const ACE &ace)
{
    (void)fprintf(stderr, "ACE TYPE: %d :: ", ace.type);
    fprintf_binary(stderr, ace.type);

    switch (ace.type) {
    case ACCESS_ALLOWED_ACE_TYPE:
        qDebug() << "ACCESS_ALLOWED_ACE_TYPE";
        break;
    case ACCESS_DENIED_ACE_TYPE:
        qDebug() << "ACCESS_DENIED_ACE_TYPE";
        break;
    case SYSTEM_AUDIT_ACE_TYPE:
        qDebug() << "SYSTEM_AUDIT_ACE_TYPE";
        break;
    case SYSTEM_ALARM_ACE_TYPE:
        qDebug() << "SYSTEM_ALARM_ACE_TYPE";
        break;
    case SYSTEM_MANDATORY_LABEL_ACE_TYPE:
        qDebug() << "SYSTEM_MANDATORY_LABEL_ACE_TYPE";
        break;
    }

    // There's actually more, albeit not in winnt.h
    // Unclear if modeled inside samba.
    // https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-ace_header
}

QString check(unsigned int value)
{
    if (value) {
        return QStringLiteral("☒");
    }
    return QStringLiteral("☐");
}

void printMaskValue(const std::string_view name, unsigned int value)
{
    qDebug() << qUtf8Printable(check(value)) << name.data();
}

#define MASK_VALUE(name) printMaskValue(#name, (m & name))

void printFlags(const ACE &ace)
{
    (void)fprintf(stderr, "ACE FLAGS: %d :: ", ace.flags);
    fprintf_binary(stderr, ace.flags);

    // https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-ace_header
    // https://docs.microsoft.com/en-us/windows/win32/wmisdk/namespace-ace-flag-constants
    // https://docs.microsoft.com/en-us/windows/win32/secauthz/ace-inheritance-rules
    const auto m = ace.flags;
    MASK_VALUE(OBJECT_INHERIT_ACE);
    MASK_VALUE(CONTAINER_INHERIT_ACE);
    MASK_VALUE(FAILED_ACCESS_ACE_FLAG);
    MASK_VALUE(INHERIT_ONLY_ACE);
    MASK_VALUE(INHERITED_ACE);
    MASK_VALUE(NO_PROPAGATE_INHERIT_ACE);
    MASK_VALUE(SUCCESSFUL_ACCESS_ACE_FLAG);
}

void printMask(const ACE &ace)
{
    (void)fprintf(stderr, "ACE MASK: %d :: ", ace.mask);
    fprintf_binary(stderr, ace.mask);

    const auto m = ace.mask;

    // https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-rprn/945c4df5-4969-49c5-b1ce-59c1b3f35024
    MASK_VALUE(DELETE);
    MASK_VALUE(READ_CONTROL);
    MASK_VALUE(WRITE_DAC);
    MASK_VALUE(WRITE_OWNER);
    MASK_VALUE(SYNCHRONIZE);
    MASK_VALUE(STANDARD_RIGHTS_REQUIRED);
    MASK_VALUE(STANDARD_RIGHTS_READ);
    MASK_VALUE(STANDARD_RIGHTS_WRITE);
    MASK_VALUE(STANDARD_RIGHTS_EXECUTE);
    MASK_VALUE(STANDARD_RIGHTS_ALL);
    MASK_VALUE(SPECIFIC_RIGHTS_ALL);
    MASK_VALUE(GENERIC_READ);
    MASK_VALUE(GENERIC_WRITE);
    MASK_VALUE(GENERIC_EXECUTE);
    MASK_VALUE(GENERIC_ALL);
    MASK_VALUE(MAXIMUM_ALLOWED);
    MASK_VALUE(ACCESS_SYSTEM_SECURITY);

    // -------------------------------------------------

    // https://docs.microsoft.com/en-us/windows/win32/api/iaccess/nf-iaccess-iaccesscontrol-isaccessallowed
    MASK_VALUE(ACTRL_ACCESS_ALLOWED);
    MASK_VALUE(ACTRL_ACCESS_DENIED);
    MASK_VALUE(ACTRL_AUDIT_SUCCESS);
    MASK_VALUE(ACTRL_AUDIT_FAILURE);
    MASK_VALUE(ACTRL_ACCESS_PROTECTED);
    MASK_VALUE(ACTRL_SYSTEM_ACCESS);
    MASK_VALUE(ACTRL_DELETE);
    MASK_VALUE(ACTRL_READ_CONTROL);
    MASK_VALUE(ACTRL_CHANGE_ACCESS);
    MASK_VALUE(ACTRL_CHANGE_OWNER);
    MASK_VALUE(ACTRL_SYNCHRONIZE);
    MASK_VALUE(ACTRL_STD_RIGHTS_ALL);
    MASK_VALUE(ACTRL_STD_RIGHT_REQUIRED);

    MASK_VALUE(ACTRL_DS_OPEN);
    MASK_VALUE(ACTRL_DS_CREATE_CHILD);
    MASK_VALUE(ACTRL_DS_DELETE_CHILD);
    MASK_VALUE(ACTRL_DS_LIST);
    MASK_VALUE(ACTRL_DS_SELF);
    MASK_VALUE(ACTRL_DS_READ_PROP);
    MASK_VALUE(ACTRL_DS_WRITE_PROP);
    MASK_VALUE(ACTRL_DS_DELETE_TREE);
    MASK_VALUE(ACTRL_DS_LIST_OBJECT);
    MASK_VALUE(ACTRL_DS_CONTROL_ACCESS);

    MASK_VALUE(ACTRL_FILE_READ);
    MASK_VALUE(ACTRL_FILE_WRITE);
    MASK_VALUE(ACTRL_FILE_APPEND);
    MASK_VALUE(ACTRL_FILE_READ_PROP);
    MASK_VALUE(ACTRL_FILE_WRITE_PROP);
    MASK_VALUE(ACTRL_FILE_EXECUTE);
    MASK_VALUE(ACTRL_FILE_READ_ATTRIB);
    MASK_VALUE(ACTRL_FILE_WRITE_ATTRIB);
    MASK_VALUE(ACTRL_FILE_CREATE_PIPE);

    MASK_VALUE(ACTRL_DIR_LIST);
    MASK_VALUE(ACTRL_DIR_CREATE_OBJECT);
    MASK_VALUE(ACTRL_DIR_CREATE_CHILD);
    MASK_VALUE(ACTRL_DIR_DELETE_CHILD);
    MASK_VALUE(ACTRL_DIR_TRAVERSE);

    // -------------------------------------------------

    // NOTE: samba/librpc/idl/security.idl has also values but they seem duplicates
    //  of the windows ones. We should probably make a decision on what to use,
    //  wine is probably smarter from a code reuse perspective, the samba stuff
    //  isn't available in headers.
}

void printACE(const ACE &ace)
{
    printType(ace);
    printFlags(ace);
    printMask(ace);
}
