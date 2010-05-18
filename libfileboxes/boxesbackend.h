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
#ifndef BOXESBACKEND_HEADER
#define BOXESBACKEND_HEADER
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include "Nepomuk/Resource"
#include "boxsettings.h"
#include "fileboxes_export.h"
class FILEBOXES_EXPORT BoxesBackend
{
public:
    BoxesBackend();
    ~BoxesBackend();
    bool newFile(const QString &fileName, const QString &boxID);
    QString newBox(const QString &boxName, const QString &icon);

    bool removeBox(const QString &boxID);
    bool removeFile(const QString& fileName, const QString& boxID);
    bool removeFiles(const QStringList &fileNames, const QString &boxID);
    bool removeAllFiles(const QString &boxID);

    QStringList boxIDs();
    QStringList boxNames();
    QList<QUrl> files(const QString &boxID);

    QString name(const QString &boxID);
    QString icon(const QString &boxID);
    bool hasNew(const QString &boxID);
    void setHasNew(const bool &hasNew, const QString &boxID);

    unsigned int boxSize(const QString &boxID);
    QString localPath(QUrl url);
    QUrl localUrl(QUrl url);
    Nepomuk::Resource boxRes(const QString& boxID);
    QString boxResUrl(const QString& boxID);
    int m_loaded;

private:
    const Nepomuk::Resource fileBoxesRes();

};

#endif
