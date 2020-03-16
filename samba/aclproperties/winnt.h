// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#pragma once

// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-ace_header
// From winnt.h in wine

/* AceType */
#define ACCESS_ALLOWED_ACE_TYPE 0
#define ACCESS_DENIED_ACE_TYPE 1
#define SYSTEM_AUDIT_ACE_TYPE 2
#define SYSTEM_ALARM_ACE_TYPE 3
#define SYSTEM_MANDATORY_LABEL_ACE_TYPE 0x11

/* inherit AceFlags */
#define OBJECT_INHERIT_ACE 0x01
#define CONTAINER_INHERIT_ACE 0x02
#define NO_PROPAGATE_INHERIT_ACE 0x04
#define INHERIT_ONLY_ACE 0x08
#define INHERITED_ACE 0x10
#define VALID_INHERIT_FLAGS 0x1F

/* AceFlags mask for what events we (should) audit */
#define SUCCESSFUL_ACCESS_ACE_FLAG 0x40
#define FAILED_ACCESS_ACE_FLAG 0x80

/* DELETE may be already defined via /usr/include/arpa/nameser_compat.h */
#undef DELETE
#define DELETE 0x00010000
#define READ_CONTROL 0x00020000
#define WRITE_DAC 0x00040000
#define WRITE_OWNER 0x00080000
#define SYNCHRONIZE 0x00100000
#define STANDARD_RIGHTS_REQUIRED 0x000f0000

#define STANDARD_RIGHTS_READ READ_CONTROL
#define STANDARD_RIGHTS_WRITE READ_CONTROL
#define STANDARD_RIGHTS_EXECUTE READ_CONTROL

#define STANDARD_RIGHTS_ALL 0x001f0000

#define SPECIFIC_RIGHTS_ALL 0x0000ffff

#define GENERIC_READ 0x80000000
#define GENERIC_WRITE 0x40000000
#define GENERIC_EXECUTE 0x20000000
#define GENERIC_ALL 0x10000000

#define MAXIMUM_ALLOWED 0x02000000
#define ACCESS_SYSTEM_SECURITY 0x01000000
