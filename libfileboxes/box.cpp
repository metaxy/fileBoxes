/********************************************************************************
** Copyright 2009-2010 Paul Walger metaxy@walger.name
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) version 3, or any
** later version accepted by the membership of KDE e.V. (or its
** successor approved by the membership of KDE e.V.), which shall
** act as a proxy defined in Section 6 of version 3 of the license.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************************/
#include "box.h"
#include <KLocale>
#include <QtCore/QtDebug>
#include <KUrl>
#include <KDirWatch>
#include <kdirnotify.h>
Box::Box(const QString &boxID, const QString & name, const QString & icon)
{
    m_boxID = boxID;
    if(m_boxID.isEmpty()) {
        //create new box
        m_boxID = m_backend.newBox(name, icon);
    }
}
bool Box::addFiles(const QStringList &files)
{
    qDebug() << Q_FUNC_INFO << files;
    for(int i = 0; i < files.size(); ++i) {
        m_backend.newFile(files.at(i), m_boxID);
    }
    org::kde::KDirNotify::emitFilesAdded("fileboxes:/" + m_boxID + "/");
    return true;
}
bool Box::removeFiles(const QStringList &files)
{
    bool ret = m_backend.removeFiles(files, m_boxID);
    //org::kde::KDirNotify::emitFilesRemoved(QStringList() << "fileboxes:/"+m_boxID+"/");
    return ret;
}
bool Box::removeAllFiles()
{
    bool ret = m_backend.removeAllFiles(m_boxID);
    org::kde::KDirNotify::emitFilesRemoved(QStringList() << "fileboxes:/" + m_boxID + "/");
    return ret;
}
QString Box::name()
{
    return m_backend.name(m_boxID);
}
QString Box::boxID()
{
    return m_boxID;
}
unsigned int Box::size()
{
    return m_backend.boxSize(m_boxID);
}
QString Box::icon()
{
    return m_backend.icon(m_boxID);
}
void Box::setHasNew(const bool &hasNew)
{
    m_backend.setHasNew(hasNew, m_boxID);
}
bool Box::hasNew()
{
    return m_backend.hasNew(m_boxID);
}
bool Box::removeBox()
{
    return m_backend.removeBox(m_boxID);
}
QList<QUrl> Box::getFiles()
{
    QList<QUrl> urls;
    qDebug() << Q_FUNC_INFO << urls;
    foreach(QUrl url, m_backend.files(m_boxID)) {
        urls << m_backend.localPath(url);
    }
    return urls;
}
