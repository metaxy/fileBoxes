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
#include <nepomuk/resourcetypeterm.h>
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
    
    Resource b = boxRes(boxID);
    b.setLabel(name(boxID));
    
    Resource f(fileName);

    f.addProperty(Nepomuk::Vocabulary::NIE::isPartOf(), b);

    return true;
}
bool BoxesBackend::removeFile(const QString &fileName, const QString &boxID)
{
    Resource f(fileName);
    f.removeProperty(Nepomuk::Vocabulary::NIE::isPartOf(), boxRes(boxID));
    return true;
}
bool BoxesBackend::removeFiles(const QStringList &files, const QString &boxID)
{
    for (int i = 0; i < files.size(); ++i) {
        removeFile(files.at(i), boxID);
    }
    return true;
}
bool BoxesBackend::removeAllFiles(const QString &boxID)
{
    Nepomuk::Query::ResourceTerm partTerm(boxRes(boxID));
    Nepomuk::Query::ComparisonTerm term(Nepomuk::Vocabulary::NIE::isPartOf(),
                                        partTerm,
                                        Nepomuk::Query::ComparisonTerm::Equal);
    Nepomuk::Query::Query query(term);
    QString q = query.toSparqlQuery();
    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery(q, Soprano::Query::QueryLanguageSparql);
    while (it.next()) {
        Resource f(it.binding(0).uri());
        f.removeProperty(Nepomuk::Vocabulary::NIE::isPartOf(), boxRes(boxID));
    }
    return true;
}
QList<QUrl> BoxesBackend::files(const QString &boxID)
{
    qDebug() << "files";
    Nepomuk::Query::ResourceTerm partTerm(boxRes(boxID));
    Nepomuk::Query::ComparisonTerm term(Nepomuk::Vocabulary::NIE::isPartOf(),
                                        partTerm,
                                        Nepomuk::Query::ComparisonTerm::Equal);
    Nepomuk::Query::Query query(term);
    QString q = query.toSparqlQuery();
    qDebug() << q;
    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery(q, Soprano::Query::QueryLanguageSparql);
    QList<QUrl> urls;
    while (it.next()) {
        Resource f(it.binding(0).uri());
        urls << it.binding(0).uri();
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

    return boxID;
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
    m_settings->endGroup();
    return set;
}
bool BoxesBackend::setSettings(const QString &boxID, const struct BoxSettings &set)
{
    m_settings->beginGroup(boxID);
    m_settings->setValue("name", set.name);
    m_settings->setValue("icon", set.icon);
    m_settings->setValue("hasNew", set.hasNew);
    m_settings->endGroup();
    //sync();
    return false;
}
unsigned int BoxesBackend::boxSize(const QString &boxID)
{
    return files(boxID).size();
}

bool BoxesBackend::removeBox(const QString &boxID)
{
    removeAllFiles(boxID);
    m_settings->remove(boxID);
}
void BoxesBackend::sync()
{
    m_settings->sync();
}

Nepomuk::Resource BoxesBackend::boxRes(const QString &boxID)
{
    QUrl boxUrl("fileboxes:/" + boxID);
    Resource res(boxUrl);
    return res;
}
QString BoxesBackend::localPath(QUrl url)
{
    Resource f(url);
    return f.property(Nepomuk::Vocabulary::NIE::url()).toUrl().toLocalFile();
}
QUrl BoxesBackend::localUrl(QUrl url)
{
    Resource f(url);
    return f.property(Nepomuk::Vocabulary::NIE::url()).toUrl();
}