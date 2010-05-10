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
#include "boxesbackend.h"
#include <QtCore/QCryptographicHash>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDir>
#include <QtCore/QtDebug>
#include "Nepomuk/Resource"
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Variant>
#include <Nepomuk/Tag>
#include <nepomuk/comparisonterm.h>
#include <nepomuk/literalterm.h>
#include "/home/kde-devel/kde/include/Soprano/Vocabulary/NAO"
#include "nie.h"
using namespace Nepomuk;
BoxesBackend::BoxesBackend()
{
    m_fileBoxesHome = QDir::homePath() + "/.fileboxes";
    m_settings = new QSettings(m_fileBoxesHome + "/files.ini", QSettings::IniFormat);
}
BoxesBackend::~BoxesBackend()
{
    delete m_settings;
}
bool BoxesBackend::newFile(const QString &boxID, const QString &fileName)
{
  qDebug() << " newFile boxID = " << boxID << " fileName = " << fileName;
    QUrl boxUrl("fileboxes:/"+boxID);
        
    Resource f( fileName );
 
    
    QList<Resource> parts = f.property( Nepomuk::Vocabulary::NIE::hasPart() ).toResourceList();
    QListIterator<Resource> it_parts( parts );
    while( it_parts.hasNext() )
        qDebug() << "File tagged with part: " << it_parts.next().genericLabel();
    
       Resource boxRes( boxUrl );
    f.addProperty( Nepomuk::Vocabulary::NIE::isPartOf(), "fileboxes:/"+boxID );
    
QList<Resource> tags = f.property( Soprano::Vocabulary::NAO::hasTag() ).toResourceList();
QListIterator<Resource> it( tags );
while( it.hasNext() )
  qDebug() << "File tagged with tag 2: "
            << it.next().genericLabel();
        
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(fileName.toLocal8Bit());
    QString filePath = m_fileBoxesHome + "/" + boxID + "/" + hash.result().toHex();

    BoxSettings set = settings(boxID);
    set.places[filePath] = QVariant(fileName);
    setSettings(boxID, set);

    QFileInfo info(fileName);
    if (info.isDir()) {
        QDir dir;
        dir.mkpath(filePath);
    } else {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }
        QTextStream out(&file);
        out << "\n";
    }
    return true;
}
bool BoxesBackend::removeFile(const QString &boxID, const QString &fileName, const bool &isFile)
{
    QDir dir("/");
    bool error = false;
    if (isFile) {
        error = dir.remove(fileName);
    } else {
        error = dir.rmdir(fileName);
    }

    BoxSettings set = settings(boxID);
    set.places[fileName].clear();
    setSettings(boxID, set);
    return error;
}
QString BoxesBackend::newBox(const QString &name, const QString &icon)
{
    QStringList childGroups = boxIDs();
    int max = 0;
    for (int i = 0; i < childGroups.size(); ++i) {
        int current = childGroups.at(i).toInt();
        if (current > max) {
            max = current;
        }
    }
    QString boxID = QString::number(max + 1);
    BoxSettings set;
    set.id = boxID;
    set.name = name;
    set.icon = icon;
    set.hasNew = false;
    setSettings(boxID, set);

    QDir a;
    a.mkpath(m_fileBoxesHome + "/" + boxID + "/");

    return QString(boxID);
}
QStringList BoxesBackend::boxIDs()
{
    return m_settings->childGroups();
}
QStringList BoxesBackend::boxNames()
{
    QStringList id = boxIDs();
    QStringList ret;
    for (int i = 0; i < id.size(); ++i) {
        ret << name(id.at(i));
    }
    return ret;
}
QList<QUrl> BoxesBackend::files(const QString &boxID)
{
    QList<QVariant> list = places(boxID).values();
    QList<QUrl> urls;
    for (int i = 0; i < list.size(); ++i) {
        urls << QUrl::fromLocalFile(list.at(i).toString());
    }
    return urls;
}
QString BoxesBackend::name(const QString &boxID)
{
    return settings(boxID).name;
}
QString BoxesBackend::icon(const QString &boxID)
{
    return settings(boxID).icon;
}
QMap<QString, QVariant> BoxesBackend::places(const QString &boxID)
{
    return settings(boxID).places;
}
bool BoxesBackend::hasNew(const QString &boxID)
{
    return settings(boxID).hasNew;
}
void BoxesBackend::setHasNew(const bool &hasNew, const QString &boxID)
{
    BoxSettings set = settings(boxID);
    set.hasNew = hasNew;
    setSettings(boxID, set);
}
struct BoxSettings BoxesBackend::settings(const QString &boxID) {
    BoxSettings set;
    m_settings->beginGroup(boxID);
    set.id = boxID;
    set.name = m_settings->value("name").toString();
    set.icon = m_settings->value("icon").toString();
    set.hasNew = m_settings->value("hasNew").toBool();
    set.places = m_settings->value("places").toMap();
    m_settings->endGroup();
    return set;
}
bool BoxesBackend::setSettings(const QString &boxID, const struct BoxSettings &set)
{
    m_settings->beginGroup(boxID);
    m_settings->setValue("name", set.name);
    m_settings->setValue("icon", set.icon);
    m_settings->setValue("hasNew", set.hasNew);
    m_settings->setValue("places", set.places);
    m_settings->endGroup();
    //sync();
    return false;
}
unsigned int BoxesBackend::boxSize(const QString &boxID)
{
    return settings(boxID).places.size();
}
bool BoxesBackend::removeFiles(const QStringList &files, const QString &boxID)
{
    QDir d(m_fileBoxesHome + "/" + boxID + "/");
    for (int i = 0; i < files.size(); ++i) {
        d.remove(files.at(i));
    }
    return true;
    //todo:remove in places
}
bool BoxesBackend::removeAllFiles(const QString &boxID)
{
    //clear places
    BoxSettings set = settings(boxID);
    set.places.clear();
    setSettings(boxID, set);
    //remove dir
    rm(m_fileBoxesHome + "/" + boxID+"/");
    //make dir
    QDir dir;
    return dir.mkpath(m_fileBoxesHome + "/" + boxID);
}
QString BoxesBackend::fileBoxesHome()
{
    return m_fileBoxesHome;
}
bool BoxesBackend::removeBox(const QString &boxID)
{
    m_settings->remove(boxID);//remove from settings
    //remove dir
    QDir dir(m_fileBoxesHome + "/");
    return dir.rmpath(m_fileBoxesHome + "/" + boxID);
}
void BoxesBackend::sync()
{
    m_settings->sync();
}
void BoxesBackend::rm(const QString &path)
{
    qDebug() << "path  = " << path;
    QFileInfo fileInfo(path);
    if(fileInfo.isDir()){
        QDir dir(path);
        QStringList fileList = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
        for(int i = 0; i < fileList.count(); ++i) {
            rm(path+fileList.at(i));
        }
        dir.rmdir(path);
    } else {
        QFile::remove(path);
    }
}