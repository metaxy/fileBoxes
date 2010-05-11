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
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Variant>
#include <Nepomuk/Tag>
#include <nepomuk/query.h>
#include <nepomuk/resourceterm.h>
#include <nepomuk/comparisonterm.h>
#include <nepomuk/literalterm.h>
#include "nao.h"
#include "nie.h"
#include "nfo.h"
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Client/DBusClient>
#include <Soprano/Client/DBusModel>
using namespace Nepomuk;
BoxesBackend::BoxesBackend()
{
    m_fileBoxesHome = QDir::homePath() + "/.fileboxes";
    m_settings = new QSettings(m_fileBoxesHome + "/files.ini", QSettings::IniFormat);
    Nepomuk::ResourceManager::instance()->init();
    qDebug() << "Version 0.1.3";
}
BoxesBackend::~BoxesBackend()
{
    delete m_settings;
}
bool BoxesBackend::newFile(const QString &boxID, const QString &fileName)
{
    qDebug() << " newFile boxID = " << boxID << " fileName = " << fileName;
   
    /* Nepomuk Test */
    //_________________________________________________________________________________________-        
    Resource f( fileName );

    QList<Resource> parts = f.property( Nepomuk::Vocabulary::NIE::isPartOf() ).toResourceList();
    QListIterator<Resource> it_parts( parts );
    while( it_parts.hasNext() )
        qDebug() << "File hasPart : " << it_parts.next().genericLabel();
    //boxRes.setLabel(name(boxID));
    f.addProperty( Nepomuk::Vocabulary::NIE::isPartOf(), boxRes(boxID) );
    
    QList<Resource> tags = f.property( Soprano::Vocabulary::NAO::hasTag() ).toResourceList();
    QListIterator<Resource> it( tags );
    while( it.hasNext() )
        qDebug() << "File tagged with tag 2: " << it.next().genericLabel();
    //_________________________________________________________________________________________-    
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
    files(boxID);
    return true;
}
bool BoxesBackend::removeFile(const QString &fileName,const QString &boxID)
{
    Resource f( fileName );
    f.removeProperty(Nepomuk::Vocabulary::NIE::isPartOf(), boxRes(boxID));
    //_______________________________________________________________
      
   /* QDir dir;
    bool error = false;
    if (isFile) {
        error = dir.remove(fileName);
    } else {
        error = dir.rmdir(fileName);
    }

    BoxSettings set = settings(boxID);
    set.places[fileName].clear();
    setSettings(boxID, set);*/
    return true;
}
bool BoxesBackend::removeFiles(const QStringList &files, const QString &boxID)
{
    QDir d(m_fileBoxesHome + "/" + boxID + "/");
    for (int i = 0; i < files.size(); ++i) {
        removeFile(files.at(i),boxID);
    }
    return true;
    //todo:remove in places
}
bool BoxesBackend::removeAllFiles(const QString &boxID)
{
    Nepomuk::Query::ResourceTerm partTerm( boxRes(boxID) );
    Nepomuk::Query::ComparisonTerm term( Nepomuk::Vocabulary::NIE::isPartOf(), 
                                     partTerm, 
                                     Nepomuk::Query::ComparisonTerm::Equal);
    Nepomuk::Query::Query query( term );
    QString q = query.toSparqlQuery();
    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery( q, Soprano::Query::QueryLanguageSparql );
    while( it.next() ) {
        Resource f( it.binding(0).uri());
        f.removeProperty(Nepomuk::Vocabulary::NIE::isPartOf(), boxRes(boxID));
    }
    //_________________________________________________________________
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
QList<QUrl> BoxesBackend::files(const QString &boxID)
{
    qDebug() << "files";
    Nepomuk::Query::ResourceTerm partTerm( boxRes(boxID) );
    Nepomuk::Query::ComparisonTerm term( Nepomuk::Vocabulary::NIE::isPartOf(), 
                                     partTerm, 
                                     Nepomuk::Query::ComparisonTerm::Equal);
    Nepomuk::Query::Query query( term );
    QString q = query.toSparqlQuery();
    qDebug() << q;
    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery( q, Soprano::Query::QueryLanguageSparql );
    while( it.next() ) {
        Resource f( it.binding(0).uri());
        qDebug() << it.binding(0).uri() << f.property(Nepomuk::Vocabulary::NIE::url());
    }
    //________________________________________________________________________
    QList<QVariant> list = places(boxID).values();
    QList<QUrl> urls;
    for (int i = 0; i < list.size(); ++i) {
        urls << QUrl::fromLocalFile(list.at(i).toString());
    }
    return urls;
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

QString BoxesBackend::fileBoxesHome()
{
    return m_fileBoxesHome;
}
bool BoxesBackend::removeBox(const QString &boxID)
{
    removeAllFiles(boxID);
    //_____________________________________________________
    m_settings->remove(boxID);//remove from settings
    //remove dir
    QDir dir(m_fileBoxesHome + "/");
    return dir.rmpath(m_fileBoxesHome + "/" + boxID);
}
void BoxesBackend::sync()
{
    m_settings->sync();
}

Nepomuk::Resource BoxesBackend::boxRes(const QString &boxID)
{
    QUrl boxUrl("fileboxes:/"+boxID);
    Resource res( boxUrl );
    return res;
}

void BoxesBackend::rm(const QString &path)
{
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