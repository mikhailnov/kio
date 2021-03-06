/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

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

#include <QCoreApplication>
#include <QDebug>
#include <QUrl>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDataStream>
#include <KLocalizedString>
#include <KIO/EmptyTrashJob>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("ktrash"));
    app.setApplicationVersion(QStringLiteral(PROJECT_VERSION));
    app.setOrganizationDomain(QStringLiteral("kde.org"));

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription(i18n("Helper program to handle the KDE trash can\n"
                                          "Note: to move files to the trash, do not use ktrash, but \"kioclient move 'url' trash:/\""));

    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("empty"), i18n("Empty the contents of the trash")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("restore"), i18n("Restore a trashed file to its original location"), QStringLiteral("file")));

    parser.process(app);

    if (parser.isSet(QStringLiteral("empty"))) {
        // We use a kio job instead of linking to TrashImpl, for a smaller binary
        // (and the possibility of a central service at some point)
        KIO::Job *job = KIO::emptyTrash();
        job->exec();
        return 0;
    }

    QString restoreArg = parser.value(QStringLiteral("restore"));
    if (!restoreArg.isEmpty()) {

        if (restoreArg.indexOf(QLatin1String("system:/trash")) == 0) {
            restoreArg.replace(0, 13, QStringLiteral("trash:"));
        }

        QUrl trashURL(restoreArg);
        if (!trashURL.isValid() || trashURL.scheme() != QLatin1String("trash")) {
            qCritical() << "Invalid URL for restoring a trashed file, trash:// URL expected:" << trashURL;
            return 1;
        }

        QByteArray packedArgs;
        QDataStream stream(&packedArgs, QIODevice::WriteOnly);
        stream << (int)3 << trashURL;
        KIO::Job *job = KIO::special(trashURL, packedArgs);
        bool ok = job->exec() ? true : false;
        if (!ok) {
            qCritical() << job->errorString();
        }
        return 0;
    }

    return 0;
}
