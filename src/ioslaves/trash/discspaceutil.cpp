/*
   This file is part of the KDE project

   Copyright (C) 2008 Tobias Koenig <tokoe@kde.org>

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

#include "discspaceutil.h"
#include "kiotrashdebug.h"

#include <QDirIterator>
#include <QFileInfo>

#include <kdiskfreespaceinfo.h>
#include <QDebug>
#include <qplatformdefs.h> // QT_LSTAT, QT_STAT, QT_STATBUF

DiscSpaceUtil::DiscSpaceUtil(const QString &directory)
    : mDirectory(directory),
      mFullSize(0)
{
    calculateFullSize();
}

qulonglong DiscSpaceUtil::sizeOfPath(const QString &path)
{
    QFileInfo info(path);
    if (!info.exists()) {
        return 0;
    }

    if (info.isSymLink()) {
        // QFileInfo::size does not return the actual size of a symlink. #253776
        QT_STATBUF buff;
        return static_cast<qulonglong>(QT_LSTAT(QFile::encodeName(path).constData(), &buff) == 0 ? buff.st_size : 0);
    } else if (info.isFile()) {
        return info.size();
    } else if (info.isDir()) {
        QDirIterator it(path, QDirIterator::NoIteratorFlags);

        qulonglong sum = 0;
        while (it.hasNext()) {
            const QFileInfo info = it.next();

            if (info.fileName() != QLatin1String(".") && info.fileName() != QLatin1String("..")) {
                sum += sizeOfPath(info.absoluteFilePath());
            }
        }

        return sum;
    } else {
        return 0;
    }
}

double DiscSpaceUtil::usage(qulonglong size) const
{
    if (mFullSize == 0) {
        return 0;
    }

    return (((double)size * 100) / (double)mFullSize);
}

qulonglong DiscSpaceUtil::size() const
{
    return mFullSize;
}

QString DiscSpaceUtil::mountPoint() const
{
    return mMountPoint;
}

void DiscSpaceUtil::calculateFullSize()
{
    KDiskFreeSpaceInfo info = KDiskFreeSpaceInfo::freeSpaceInfo(mDirectory);
    if (info.isValid()) {
        mFullSize = info.size();
        mMountPoint = info.mountPoint();
    }
}
