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
#include <KIO/Job>
#include <KIO/NetAccess>

#include <QCoreApplication>
FileBoxesProtocol::FileBoxesProtocol(const QByteArray& protocol, const QByteArray &pool, const QByteArray &app)
        : KIO::ForwardingSlaveBase(protocol, pool, app)
{
    m_backend = new BoxesBackend();
}

FileBoxesProtocol::~FileBoxesProtocol()
{
}
int FileBoxesProtocol::parseUrl(const KUrl& url)
{
    kDebug() << url;
    const QString path = url.path(KUrl::RemoveTrailingSlash);
    if (path.isEmpty() || path == QLatin1String("/")) {
        return 0;
    } else {
        return 1;
    }
}
KIO::UDSEntry FileBoxesProtocol::createBox(const QString& boxID)
{
    KIO::UDSEntry uds;
    uds.insert(KIO::UDSEntry::UDS_NAME, boxID);
    uds.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, m_backend->name(boxID));
    uds.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR);
    uds.insert(KIO::UDSEntry::UDS_MIME_TYPE, QString::fromLatin1("inode/directory"));
    uds.insert(KIO::UDSEntry::UDS_ICON_NAME, m_backend->icon(boxID));
    uds.insert(KIO::UDSEntry::UDS_TARGET_URL, "fileboxes:/" + boxID);
    uds.insert(KIO::UDSEntry::UDS_SIZE , i18n("%1 items", QString::number(m_backend->boxSize(boxID))));
    uds.insert(KIO::UDSEntry::UDS_NEPOMUK_URI, m_backend->boxResUrl(boxID));
    return uds;
}
KIO::UDSEntry FileBoxesProtocol::createLink(QUrl url)
{
    KIO::UDSEntry uds;
    if (KIO::StatJob* job = KIO::stat(url, KIO::HideProgressInfo)) {
        job->setAutoDelete(false);
        if (KIO::NetAccess::synchronousRun(job, 0)) {
            uds = job->statResult();
        }
        delete job;
    }
    QUrl localUrl = m_backend->localUrl(url);
    QString localPath = localUrl.toLocalFile();
    KUrl fileUrl(localUrl);
    if (uds.isDir()) {
        uds.insert(KIO::UDSEntry::UDS_URL, fileUrl.url());
    }
//
    if (fileUrl.isLocalFile()) {
        uds.insert(KIO::UDSEntry::UDS_LOCAL_PATH, fileUrl.toLocalFile());
    }
    uds.insert(KIO::UDSEntry::UDS_NEPOMUK_URI, url.toString());
    KUrl u(url);
    uds.insert(KIO::UDSEntry::UDS_NAME,  QString::fromAscii(u.toEncoded().toPercentEncoding(QByteArray(), QByteArray(""), '_')));


    KFileItem item(KFileItem::Unknown, KFileItem::Unknown, KUrl(localPath), false);
    /* QString ort = localPath;
     if (ort.startsWith(QDir::homePath())) {
         ort.remove(0,QDir::homePath().size()+1);
     }

     if (localPath == QDir::homePath()+"/"+item.name()) {
         ort = QDir::homePath();
     }

     if (ort.endsWith(item.name())) {
         ort.remove(ort.size()-item.name().size()-1,item.name().size()+1);
     }

     if (ort == "" || ort == "/") {
         uds.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, item.name());
     }
     else {
         uds.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, item.name()+"\n"+ort);
     }
    */
    uds.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, item.name());
    QString mimetype = uds.stringValue(KIO::UDSEntry::UDS_MIME_TYPE);
    if (mimetype.isEmpty()) {
        mimetype = KMimeType::findByUrl(fileUrl)->name();
        uds.insert(KIO::UDSEntry::UDS_MIME_TYPE, mimetype);
    }

    if (item.isFile()) {
        uds.insert(KIO::UDSEntry::UDS_LOCAL_PATH, localPath);
        uds.insert(KIO::UDSEntry::UDS_TARGET_URL, localPath);
        uds.insert(KIO::UDSEntry::UDS_LINK_DEST, localPath);
    }
    return uds;
}
void FileBoxesProtocol::listDir(const KUrl& url)
{
    if(m_backend->m_loaded == -1) {
        delete m_backend;
        m_backend = 0;
        m_backend = new BoxesBackend();
    }
    if(m_backend->m_loaded == -1) {
        listEntry(KIO::UDSEntry(), true);
        finished();
        //todo: return error nepomuk not laoded
        return;
    }
    
    if (parseUrl(url) == 0) {
        //create Boxes
        QStringList boxes = m_backend->boxIDs();
        foreach(QString id, boxes) {
            listEntry(createBox(id), false);
        }
        listEntry(KIO::UDSEntry(), true);
        finished();
    } else {
        QString boxID = url.path();

        boxID.remove("fileboxes:/");
        boxID.remove("/");

        QList<QUrl> urls = m_backend->files(boxID);

        foreach(QUrl k, urls) {
            listEntry(createLink(k), false);
        }
        listEntry(KIO::UDSEntry(), true);
        finished();
    }

}
void FileBoxesProtocol::del(const KUrl& url, bool isfile)
{
    if(parseUrl(url) != 0 ) {
        QString boxID = url.path();
        boxID.remove(0, 1);
        boxID.remove(boxID.indexOf("/"), boxID.size());


        QString f = url.path();
        f.remove(0, f.lastIndexOf("/") + 1);
        f.replace("nepomuk_3A_2Fres_2F", "nepomuk:/res/");

        m_backend->removeFile(f, boxID);
    } else {
        //todo:
        qDebug() << "remove box" << url;
    }
    finished();
}
void FileBoxesProtocol::put(const KUrl& url, int permissions, KIO::JobFlags flags)
{
    //and how should i know where the file is from???
    const QString dest_orig = url.toLocalFile();
    qDebug() << "fileboxes "  << url.path()  << dest_orig;
    finished();
}
void FileBoxesProtocol::mimetype(const KUrl& url)
{
    ForwardingSlaveBase::mimetype(url);
}
void FileBoxesProtocol::stat(const KUrl& url)
{
    if (parseUrl(url) == 1) {
        QString boxID = url.path();
        boxID.remove(0, 1);
        boxID.remove(boxID.indexOf("/"), boxID.size());

        KIO::UDSEntry uds;
        uds.insert(KIO::UDSEntry::UDS_NAME, boxID);
        uds.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, m_backend->name(boxID));
        uds.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR);
        uds.insert(KIO::UDSEntry::UDS_MIME_TYPE, QString::fromLatin1("inode/directory"));
        uds.insert(KIO::UDSEntry::UDS_ICON_NAME, m_backend->icon(boxID));
        uds.insert(KIO::UDSEntry::UDS_SIZE , i18n("%1 items", QString::number(m_backend->boxSize(boxID))));
        statEntry(uds);
        finished();
    } else {
        ForwardingSlaveBase::stat(url);
    }
}
void FileBoxesProtocol::prepareUDSEntry(KIO::UDSEntry& entry,
                                        bool listing) const
{
    ForwardingSlaveBase::prepareUDSEntry(entry, listing);
}
bool FileBoxesProtocol::rewriteUrl(const KUrl& url, KUrl& newURL)
{
    qDebug() << url;
    return false;
}

extern "C" int KDE_EXPORT kdemain(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    KComponentData("kio_fileboxes", "kdelibs4");
    KGlobal::locale();
    FileBoxesProtocol slave(argv[1], argv[2], argv[3]);
    slave.dispatchLoop();
    return 0;
}

