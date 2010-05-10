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
#include <klocale.h>
#include <kfileplacesmodel.h>
#include <KApplication>
#include <KCmdLineArgs>
#include <KConfigGroup>
#include <KStandardDirs>
#include <KFileItem>
#include <kio/udsentry.h>
#include <QFile>
#include <QDBusInterface>
#include <QDesktopServices>
#include <QDir>
#include <QStringList>
#include <QMap>
#include <QVariant>
//#include <QSettings>
#include <QFileInfo>
#include <KDebug>
#include <QMapIterator>
FileBoxesProtocol::FileBoxesProtocol(const QByteArray& protocol, const QByteArray &pool, const QByteArray &app)
        : KIO::ForwardingSlaveBase(protocol, pool, app)
{
    m_backend = new BoxesBackend();
    m_fileBoxesHome = m_backend->fileBoxesHome();
}

FileBoxesProtocol::~FileBoxesProtocol()
{
}
bool FileBoxesProtocol::rewriteUrl(const KUrl &url, KUrl &newUrl)
{
    m_backend->sync();
    QString path = url.path();
    QStringList list_url = path.split("/");//bsp /home/paul/.fileboxes/1/
    QStringList list_home = m_fileBoxesHome.split("/");//bsp /home/paul/.fileboxes/
    if (list_url.size() > list_home.size()) {
        newUrl.setProtocol("file");
        QString boxID = list_url.at(list_home.size());
        QMap<QString,QVariant> places = m_backend->places(boxID);
        newUrl.setPath(places[path].toString());
    }
    else {
        newUrl.setProtocol("file");
        newUrl.setPath(m_fileBoxesHome);
        QStringList groups = m_backend->boxIDs();
        for (int i=0;i<groups.size();++i) {
            //if(backend->name(groups.at(i)) == list_url.at(1))//todo:change to id
            if (groups.at(i) == list_url.at(1)) {
                path = groups.at(i);
                break;
            }
        }
        newUrl.addPath(path);
    }
    return true;
}

void FileBoxesProtocol::prepareUDSEntry(KIO::UDSEntry &entry, bool listing) const
{
    m_backend->sync();
    bool isVirtual = false;
    ForwardingSlaveBase::prepareUDSEntry(entry, listing);
    const QString path = entry.stringValue(KIO::UDSEntry::UDS_NAME);
    QFile f(path);
    QString name = f.fileName();
    if (name == "." || name == ".." || name == "files.ini") {//dont show them
        entry.insert(KIO::UDSEntry::UDS_HIDDEN, 1);
        return;
    }
    QString localPath = entry.stringValue(KIO::UDSEntry::UDS_LOCAL_PATH);
    if (localPath.endsWith("/")) {
        localPath = localPath.remove(localPath.size()-1,2);
    }
    QStringList listPath = localPath.split("/");
    QStringList listHome = m_fileBoxesHome.split("/");
    if (/*listPath.size() > listHome.size()*/ true) {
        QString fileBoxID = listPath.at(listHome.size());
        QStringList groups(m_backend->boxIDs());
        if (groups.contains(fileBoxID) && (listPath.size()-1) == listHome.size() && entry.isDir()) {
            isVirtual = false;
            name = m_backend->name(fileBoxID);
            localPath = "fileboxes:/"+fileBoxID;
            entry.insert(KIO::UDSEntry::UDS_ICON_NAME, m_backend->icon(fileBoxID));
            entry.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, name);
            entry.insert(KIO::UDSEntry::UDS_SIZE , i18n("%1 items",QString::number(m_backend->boxSize(fileBoxID))));
            entry.insert(KIO::UDSEntry::UDS_TARGET_URL, localPath );
            entry.insert(KIO::UDSEntry::UDS_LOCAL_PATH, localPath );
        }
        else/* if(groups.contains(fileBoxID))*/ {
            isVirtual = true;
            QMap<QString,QVariant> places = m_backend->places(fileBoxID);
            QString b = localPath;
            localPath = places[b].toString();
            name = localPath;
        }
    }
    if (!name.isEmpty() && isVirtual == true) {
        QFileInfo f(localPath);
        QString ort = localPath;
        if (ort.startsWith(QDir::homePath())) {
            ort.remove(0,QDir::homePath().size()+1);
        }


        if (!f.isDir()) {
            KFileItem item(KFileItem::Unknown,KFileItem::Unknown,KUrl(localPath),false);
            
            if (localPath == QDir::homePath()+"/"+item.name()) {
                ort = QDir::homePath();
            }
            if (ort.endsWith(item.name())) {
                ort.remove(ort.size()-item.name().size()-1,item.name().size()+1);
            }
            if (ort == "" || ort == "/") {
                entry.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, item.name());
            }
            else {
                entry.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, item.name()+"\n"+ort);
            }
            
            entry.insert(KIO::UDSEntry::UDS_SIZE , item.size());
            entry.insert(KIO::UDSEntry::UDS_USER , item.user());
            entry.insert(KIO::UDSEntry::UDS_GROUP , item.group());
            entry.insert(KIO::UDSEntry::UDS_ACCESS , item.permissions());
            entry.insert(KIO::UDSEntry::UDS_MODIFICATION_TIME , item.time(KFileItem::ModificationTime).toTime_t());
            entry.insert(KIO::UDSEntry::UDS_ACCESS_TIME ,item.time(KFileItem::AccessTime).toTime_t());
            entry.insert(KIO::UDSEntry::UDS_CREATION_TIME , item.time(KFileItem::CreationTime).toTime_t());
            entry.insert(KIO::UDSEntry::UDS_MIME_TYPE , item.determineMimeType());

            entry.insert(KIO::UDSEntry::UDS_TARGET_URL, localPath );
            entry.insert(KIO::UDSEntry::UDS_LOCAL_PATH, localPath );
        }
        else {
            //todo: fix problems with directorys
            //x-directory/normal
            entry.insert(KIO::UDSEntry::UDS_MIME_TYPE , "inode/directory");
            entry.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, f.fileName());
            entry.insert(KIO::UDSEntry::UDS_TARGET_URL, localPath );
            entry.insert(KIO::UDSEntry::UDS_LOCAL_PATH, localPath );
            entry.insert(KIO::UDSEntry::UDS_LINK_DEST, localPath );
        }
    }

}
//todo:make it work
void FileBoxesProtocol::del( const KUrl &url, bool isfile )
{
    QString path = url.path();
    QStringList list = path.split("/");
    QString boxID = list.at(list.size()-1);
    if(m_backend->removeFile(boxID,path,isfile)) {
        finished();
    }
    return;
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

