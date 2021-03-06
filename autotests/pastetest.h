/* This file is part of KDE Frameworks
    Copyright (c) 2005-2006 David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KIOPASTETEST_H
#define KIOPASTETEST_H

#include <QObject>
#include <QTemporaryDir>
#include <QString>

class KIOPasteTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testPopulate();
    void testCut();
    void testPasteActionText_data();
    void testPasteActionText();
    void testPasteJob_data();
    void testPasteJob();

private:
    QTemporaryDir m_tempDir;
    QString m_dir;
};

#endif
