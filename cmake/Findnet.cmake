# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>

find_program(net_EXE net)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(net
    FOUND_VAR
        net_FOUND
    REQUIRED_VARS
        net_EXE
)
