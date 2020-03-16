// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#pragma once

#include <string>

struct ACE;

void fprintf_binary(FILE *o, uint32_t v, bool newline = true);
void printACE(const ACE &ace);
