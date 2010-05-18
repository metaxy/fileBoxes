/********************************************************************************
** Copyright 2009 Paul Walger metaxy@walger.name
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
#ifndef BOX_HEADER
#define BOX_HEADER

#include <KDE/KIcon>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QList>
#include "boxesbackend.h"
#include "fileboxes_export.h"

class FILEBOXES_EXPORT Box
{
public:
    Box(const QString &boxID, const QString &name = "Box", const QString &icon = "filebox");
    bool addFiles(const QStringList &files);
    bool removeFiles(const QStringList &files);
    bool removeAllFiles();
    QString name();
    unsigned int size();
    QString icon();
    QString boxID();
    void setHasNew(const bool &hasNew);
    bool hasNew();
    bool removeBox();
    QList<QUrl> getFiles();
    QString m_boxID;

private:
    BoxesBackend m_backend;
};

#endif
