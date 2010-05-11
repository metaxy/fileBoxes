/*
   Copyright (C) 2009 Paul Walger

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "kio_fileboxes.h"

#include <stdio.h>
#include <stdlib.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kstandarddirs.h>
#include <kcomponentdata.h>
#include <kfileplacesmodel.h>
#include <KApplication>
#include <KCmdLineArgs>
#include <KConfigGroup>
#include <KFileItem>
#include <kio/udsentry.h>
#include <QDir>
#include <QUrl>
#include <QStringList>
#include <QVariant>
#include <KUrl>
#include <kio/global.h>
#include <klocale.h>
#include <kio/job.h>
#include <KUser>
#include <KDebug>
#include <KLocale>




#include <QCoreApplication>
FileBoxesProtocol::FileBoxesProtocol(const QByteArray& protocol, const QByteArray &pool, const QByteArray &app)
        : KIO::ForwardingSlaveBase(protocol, pool, app)
{
    m_backend = new BoxesBackend();
    m_fileBoxesHome = m_backend->fileBoxesHome();
}

FileBoxesProtocol::~FileBoxesProtocol()
{
}
int FileBoxesProtocol::parseUrl( const KUrl& url )
{
    kDebug() << url;
    const QString path = url.path(KUrl::RemoveTrailingSlash);
    if( path.isEmpty() || path == QLatin1String("/") ) {
        return 0;
    }
    else {
        return 1;
    }
}
KIO::UDSEntry FileBoxesProtocol::createBox( const QString& boxID )
{
        KIO::UDSEntry uds;
         uds.insert( KIO::UDSEntry::UDS_NAME, m_backend->name(boxID) );
        uds.insert( KIO::UDSEntry::UDS_DISPLAY_NAME, m_backend->name(boxID) );
        uds.insert( KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR );
        uds.insert( KIO::UDSEntry::UDS_MIME_TYPE, QString::fromLatin1( "inode/directory" ) );
        uds.insert( KIO::UDSEntry::UDS_ICON_NAME,m_backend->icon(boxID));
        //  uds.insert( KIO::UDSEntry::UDS_MODIFICATION_TIME, dt.toTime_t() );
        //  uds.insert( KIO::UDSEntry::UDS_CREATION_TIME, dt.toTime_t() );
        uds.insert( KIO::UDSEntry::UDS_ACCESS, 0700 );
        //uds.insert( KIO::UDSEntry::UDS_USER, KUser().loginName() );
        uds.insert( KIO::UDSEntry::UDS_TARGET_URL,"fileboxes:/"+boxID);
        uds.insert( KIO::UDSEntry::UDS_SIZE , i18n("%1 items",QString::number(m_backend->boxSize(boxID))));
        return uds;
}
KIO::UDSEntry FileBoxesProtocol::createLink( QUrl url )
{
        KIO::UDSEntry uds;
        QString localPath = m_backend->localPath(url);
       
        KFileItem item(KFileItem::Unknown,KFileItem::Unknown,KUrl(localPath),false);
        uds = item.entry();

        uds.insert( KIO::UDSEntry::UDS_SIZE , item.size());
        uds.insert( KIO::UDSEntry::UDS_USER , item.user());
        uds.insert( KIO::UDSEntry::UDS_GROUP , item.group());
        uds.insert( KIO::UDSEntry::UDS_ACCESS , item.permissions());
        uds.insert( KIO::UDSEntry::UDS_MODIFICATION_TIME , item.time(KFileItem::ModificationTime).toTime_t());
        uds.insert( KIO::UDSEntry::UDS_ACCESS_TIME ,item.time(KFileItem::AccessTime).toTime_t());
        uds.insert( KIO::UDSEntry::UDS_CREATION_TIME , item.time(KFileItem::CreationTime).toTime_t());
        uds.insert( KIO::UDSEntry::UDS_MIME_TYPE , item.determineMimeType());
        uds.insert( KIO::UDSEntry::UDS_NAME, item.name() );
        uds.insert( KIO::UDSEntry::UDS_DISPLAY_NAME, item.name()  );
        uds.insert( KIO::UDSEntry::UDS_NEPOMUK_URI, url.toString() );
        
        uds.insert( KIO::UDSEntry::UDS_TARGET_URL, localPath );
        uds.insert( KIO::UDSEntry::UDS_LOCAL_PATH, localPath );
        uds.insert( KIO::UDSEntry::UDS_LINK_DEST, localPath );
        
        return uds;
}
void FileBoxesProtocol::listDir( const KUrl& url )
{
    if(parseUrl(url) == 0) {
        //create Boxes
        QStringList boxes = m_backend->boxIDs();
        foreach(QString id,boxes) {
            listEntry(createBox(id), false);
        }
        listEntry( KIO::UDSEntry(), true );
        finished();
    } else {
        //show files
        QString boxID = url.path();
        
        boxID.remove("fileboxes:/");
        boxID.remove("/");
     
        QList<QUrl> urls = m_backend->files(boxID);

        foreach(QUrl k,urls) {
            listEntry(createLink(k), false);
        }
        listEntry( KIO::UDSEntry(), true );
        finished();
    }
  
}
void FileBoxesProtocol::del( const KUrl& url, bool isfile )
{
  //  m_backend->removeFile(QUrl(url.url()));
}
void FileBoxesProtocol::put( const KUrl& url, int permissions, KIO::JobFlags flags )
{
    //todo: only if in box
        //find out box id and add file
   //m_back
}
void FileBoxesProtocol::mimetype( const KUrl& url )
{
    ForwardingSlaveBase::mimetype( url );
}
void FileBoxesProtocol::stat( const KUrl& url )
{
    ForwardingSlaveBase::stat( url );
}
void FileBoxesProtocol::prepareUDSEntry( KIO::UDSEntry& entry,
                                                 bool listing ) const
{
    ForwardingSlaveBase::prepareUDSEntry( entry, listing );
}
bool FileBoxesProtocol::rewriteUrl(const KUrl& url, KUrl& newURL)
{
    return false;
}

extern "C" int KDE_EXPORT kdemain( int argc, char **argv )
{
    QCoreApplication app(argc, argv);
    KComponentData("kio_fileboxes", "kdelibs4");
    KGlobal::locale();
    FileBoxesProtocol slave(argv[1], argv[2], argv[3]);
    slave.dispatchLoop();
    return 0;
}

