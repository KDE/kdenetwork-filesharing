// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#pragma once

#include <KPropertiesDialog>

class SambaACL : public KPropertiesDialogPlugin
{
    Q_OBJECT
    Q_PROPERTY(bool ready MEMBER m_ready NOTIFY readyChanged)
public:
    explicit SambaACL(QObject *parent, const QList<QVariant> &args);
    void applyChanges() override;

Q_SIGNALS:
    void readyChanged();

private Q_SLOTS:
    void refresh();

private:
    QUrl m_url;
    std::unique_ptr<QWidget> m_page = nullptr;
    bool m_ready = false;
};
