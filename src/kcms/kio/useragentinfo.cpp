/*
 * Copyright (c) 2001 Dawit Alemayehu <adawit@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

// Own
#include "useragentinfo.h"

// std
#include <time.h>

#ifndef Q_OS_WIN
#include <sys/utsname.h>
#else
#include <QSysInfo>
#endif

// Qt
#include <QLocale>

// KDE
#include <kservicetypetrader.h>

#define UA_PTOS(x) (*it)->property(x).toString()
#define QFL(x) QLatin1String(x)

UserAgentInfo::UserAgentInfo()
{
   m_bIsDirty = true;
}

UserAgentInfo::StatusCode UserAgentInfo::createNewUAProvider( const QString& uaStr )
{
  QStringList split;
  int pos = (uaStr).indexOf(QStringLiteral("::"));

  if ( pos == -1 )
  {
    pos = (uaStr).indexOf(QLatin1Char(':'));
    if ( pos != -1 )
    {
      split.append(uaStr.left(pos));
      split.append(uaStr.mid(pos+1));
    }
  }
  else
  {
    split = uaStr.split( QStringLiteral("::"));
  }

  if ( m_lstIdentity.contains(split[1]) )
    return DUPLICATE_ENTRY;
  else
  {
    int count = split.count();
    m_lstIdentity.append( split[1] );
    if ( count > 2 )
      m_lstAlias.append(split[2]);
    else
      m_lstAlias.append( split[1]);
  }

  return SUCCEEDED;
}

void UserAgentInfo::loadFromDesktopFiles()
{
  m_providers.clear();
  m_providers = KServiceTypeTrader::self()->query(QStringLiteral("UserAgentStrings"));
}

void UserAgentInfo::parseDescription()
{
  QString tmp;

  KService::List::ConstIterator it = m_providers.constBegin();
  KService::List::ConstIterator lastItem = m_providers.constEnd();

  for ( ; it != lastItem; ++it )
  {
    tmp = UA_PTOS(QStringLiteral("X-KDE-UA-FULL"));

    if ( (*it)->property(QStringLiteral("X-KDE-UA-DYNAMIC-ENTRY")).toBool() )
    {
#ifndef Q_OS_WIN
      struct utsname utsn;
      uname( &utsn );

      tmp.replace( QFL("appSysName"), QString::fromUtf8(utsn.sysname) );
      tmp.replace( QFL("appSysRelease"), QString::fromUtf8(utsn.release) );
      tmp.replace( QFL("appMachineType"), QString::fromUtf8(utsn.machine) );
#else
      tmp.replace( QFL("appSysName"),  QLatin1String("Windows") );
      // TODO: maybe we can use QSysInfo also on linux?
      tmp.replace( QFL("appSysRelease"), QSysInfo::kernelVersion() );
      tmp.replace( QFL("appMachineType"), QSysInfo::currentCpuArchitecture() );
#endif
      QStringList languageList = QLocale().uiLanguages();
      if ( !languageList.isEmpty() )
      {
        int ind = languageList.indexOf( QStringLiteral("C") );
        if( ind >= 0 )
        {
          if( languageList.contains( QStringLiteral("en") ) )
            languageList.removeAt( ind );
          else
            languageList.value(ind) = QStringLiteral("en");
        }
      }

      tmp.replace( QFL("appLanguage"), QStringLiteral("%1").arg(languageList.join(QStringLiteral(", "))) );
      tmp.replace( QFL("appPlatform"), QFL("X11") );
    }

    // Ignore dups...
    if ( m_lstIdentity.contains(tmp) )
      continue;

    m_lstIdentity << tmp;

    tmp = QStringLiteral("%1 %2").arg(UA_PTOS(QStringLiteral("X-KDE-UA-SYSNAME")), UA_PTOS(QStringLiteral("X-KDE-UA-SYSRELEASE")));
    if ( tmp.trimmed().isEmpty() )
      tmp = QStringLiteral("%1 %2").arg(UA_PTOS(QStringLiteral("X-KDE-UA-"
                    "NAME")), UA_PTOS(QStringLiteral("X-KDE-UA-VERSION")));
    else
      tmp = QStringLiteral("%1 %2 on %3").arg(UA_PTOS(QStringLiteral("X-KDE-UA-"
                    "NAME")), (QStringLiteral("X-KDE-UA-VERSION")), tmp);

    m_lstAlias << tmp;
  }

  m_bIsDirty = false;
}

QString UserAgentInfo::aliasStr( const QString& name )
{
  int id = userAgentStringList().indexOf(name);
  if ( id == -1 )
    return QString();
  else
    return m_lstAlias[id];
}

QString UserAgentInfo::agentStr( const QString& name )
{
  int id = userAgentAliasList().indexOf(name);
  if ( id == -1 )
    return QString();
  else
    return m_lstIdentity[id];
}


QStringList UserAgentInfo::userAgentStringList()
{
  if ( m_bIsDirty )
  {
    loadFromDesktopFiles();
    if ( m_providers.isEmpty() )
      return QStringList();
    parseDescription();
  }
  return m_lstIdentity;
}

QStringList UserAgentInfo::userAgentAliasList ()
{
  if ( m_bIsDirty )
  {
    loadFromDesktopFiles();
    if ( m_providers.isEmpty() )
      return QStringList();
    parseDescription();
  }
  return m_lstAlias;
}

