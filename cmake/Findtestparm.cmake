# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>

find_program(testparm_EXE testparm)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(testparm
    FOUND_VAR
        testparm_FOUND
    REQUIRED_VARS
        testparm_EXE
)
