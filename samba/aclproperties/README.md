<!--
    SPDX-License-Identifier: CC0-1.0
    SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>
-->

# Requirements

- samba or windows server
- samba server needs to share off of a file system with POSIX ACLs enabled.
  e.g. btrfs, or ext4 with acl enabled. also see https://help.ubuntu.com/community/FilePermissionsACLs

# Test

- browse shared folder that has ACLs enabled (`getfacl $folder` to check it has ACLs on host)
- right click empty space
- properties
- nt acl tab

# Notes

- POSIX ACL only represent a subset of NT ACL and therefore do not map terribly
  well with the windows-like UI since the three rwx bits are mapped to numerous
  windows bits
