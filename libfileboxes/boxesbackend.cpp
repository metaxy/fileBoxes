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
#include <QtCore/QtDebug>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Variant>
#include <nepomuk/query.h>
#include <nepomuk/resourceterm.h>
#include <nepomuk/comparisonterm.h>
#include <nepomuk/literalterm.h>
#include "nao.h"
#include "nie.h"
#include "nfo.h"
#include "fb.h"
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/Vocabulary/NAO>
#include <KDirWatch>
#include <QDir>
#include <kdirnotify.h>
using namespace Nepomuk;
BoxesBackend::BoxesBackend()
{
    Nepomuk::ResourceManager::instance()->init();
}
BoxesBackend::~BoxesBackend()
{
}
bool BoxesBackend::newFile(const QString& fileName, const QString& boxID)
{
    Resource f(fileName);
    f.addProperty(Nepomuk::Vocabulary::FB::isPartOfFileBox(), boxRes(boxID));

    return true;
}
QString BoxesBackend::newBox(const QString &name, const QString &icon)
{
    QString boxID = name;
    if (boxIDs().contains(name)) {
        for (int i = 1;; i++) {
            QString n(name + " (" + QString::number(i) + ")");
            if (!boxIDs().contains(n)) {
                boxID = n;
                break;
            }
        }
    }
    Resource boxR = boxRes(boxID);
    boxR.setTypes(QList<QUrl>() << Nepomuk::Vocabulary::FB::FileBox());
    boxR.setLabel(name);
    // boxR.setTypes();
    boxR.addProperty(Nepomuk::Vocabulary::FB::isFileBoxIn(), fileBoxesRes());
    boxR.addProperty(Nepomuk::Vocabulary::FB::hasNewFiles(), false);
    boxR.addProperty(Nepomuk::Vocabulary::FB::boxID(), boxID);
    boxR.addProperty(Soprano::Vocabulary::NAO::iconName() , icon);
    org::kde::KDirNotify::emitFilesAdded("fileboxes:/" + boxID + "/");

    return boxID;
}

bool BoxesBackend::removeFile(const QString &fileName, const QString &boxID)
{
    KUrl u(fileName);
    QString n = QString::fromAscii(u.toEncoded().toPercentEncoding(QByteArray(), QByteArray(""), '_'));
    Resource f(fileName);
    org::kde::KDirNotify::emitFilesRemoved(QStringList() << "fileboxes:/" + boxID + "/" + n);

    //f.removeProperty(Nepomuk::Vocabulary::NIE::isPartOf(), boxRes(boxID));
    f.removeProperty(Nepomuk::Vocabulary::FB::isPartOfFileBox(), boxRes(boxID));
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
    Nepomuk::Query::ComparisonTerm term(Nepomuk::Vocabulary::FB::isPartOfFileBox(),
                                        partTerm,
                                        Nepomuk::Query::ComparisonTerm::Equal);
    Nepomuk::Query::Query query(term);
    QString q = query.toSparqlQuery();
    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery(q, Soprano::Query::QueryLanguageSparql);
    while (it.next()) {
        Resource f(it.binding(0).uri());
        f.removeProperty(Nepomuk::Vocabulary::FB::isPartOfFileBox(), boxRes(boxID));
    }
    return true;
}
QList<QUrl> BoxesBackend::files(const QString &boxID)
{
    qDebug() << "fileboxes files = " << boxID;
    Nepomuk::Query::ResourceTerm partTerm(boxRes(boxID));
    Nepomuk::Query::ComparisonTerm term(Nepomuk::Vocabulary::FB::isPartOfFileBox(),
                                        partTerm,
                                        Nepomuk::Query::ComparisonTerm::Equal);
    Nepomuk::Query::Query query(term);
    QString q = query.toSparqlQuery();
    qDebug() << "files Query = " << q;
    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery(q, Soprano::Query::QueryLanguageSparql);
    QList<QUrl> urls;
    while (it.next()) {
        Resource f(it.binding(0).uri());
        urls << it.binding(0).uri();
    }
    qDebug() << "files result = " << urls;
    return urls;
}


QStringList BoxesBackend::boxIDs()
{
    Nepomuk::Query::ResourceTerm partTerm(fileBoxesRes());
    Nepomuk::Query::ComparisonTerm term(Nepomuk::Vocabulary::FB::isFileBoxIn(),
                                        partTerm,
                                        Nepomuk::Query::ComparisonTerm::Equal);
    Nepomuk::Query::Query query(term);
    QString q = query.toSparqlQuery();
    qDebug() << "boxIDs query = " << q;
    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery(q, Soprano::Query::QueryLanguageSparql);
    QStringList list;
    while (it.next()) {
        Resource f(it.binding(0).uri());
        list << f.property(Nepomuk::Vocabulary::FB::boxID()).toString();
    }
    qDebug() << "boxIDs result " << list;
    return list;

}
QStringList BoxesBackend::boxNames()
{
    Nepomuk::Query::ResourceTerm partTerm(fileBoxesRes());
    Nepomuk::Query::ComparisonTerm term(Nepomuk::Vocabulary::FB::isFileBoxIn(),
                                        partTerm,
                                        Nepomuk::Query::ComparisonTerm::Equal);
    Nepomuk::Query::Query query(term);
    QString q = query.toSparqlQuery();
    qDebug() << "boxNames query = " << q;
    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery(q, Soprano::Query::QueryLanguageSparql);
    QStringList list;
    while (it.next()) {
        Resource f(it.binding(0).uri());
        list << f.label();
    }
    qDebug() << "boxNames result = " << list;
    return list;
}

QString BoxesBackend::name(const QString &boxID)
{
    Resource boxR = boxRes(boxID);
    QString label = boxR.label();
    qDebug() << "boxID = " << boxID << " label = " << label;
    return label;
}

QString BoxesBackend::icon(const QString &boxID)
{
    Resource boxR = boxRes(boxID);
    QString icon = boxR.property(Soprano::Vocabulary::NAO::iconName()).toString();
    qDebug() << "boxID = " << boxID << " icon = " << icon;
    return icon;
}

bool BoxesBackend::hasNew(const QString &boxID)
{
    Resource boxR = boxRes(boxID);
    return boxR.property(Nepomuk::Vocabulary::FB::hasNewFiles()).toBool();
}
void BoxesBackend::setHasNew(const bool &hasNew, const QString &boxID)
{
    Resource boxR = boxRes(boxID);
    boxR.removeProperty(Nepomuk::Vocabulary::FB::hasNewFiles(), !hasNew);
    boxR.addProperty(Nepomuk::Vocabulary::FB::hasNewFiles(), hasNew);
}

unsigned int BoxesBackend::boxSize(const QString &boxID)
{
    return files(boxID).size();
}
bool BoxesBackend::removeBox(const QString &boxID)
{
    removeAllFiles(boxID);
    Resource boxR = boxRes(boxID);
    boxR.removeProperty(Nepomuk::Vocabulary::FB::isFileBoxIn(), fileBoxesRes());
    boxR.remove();
    org::kde::KDirNotify::emitFilesRemoved(QStringList() << "fileboxes:/" + boxID << fileBoxesRes().resourceUri().toString());
    return true;
}

Nepomuk::Resource BoxesBackend::boxRes(const QString &boxID)
{
    QUrl boxUrl("fileboxes:/" + boxID);
    Resource res(boxUrl);
    return res;
}
const Nepomuk::Resource BoxesBackend::fileBoxesRes()
{
    QUrl boxesUrl(QDir::homePath());
    Resource res(boxesUrl);
    return res;

}
QString BoxesBackend::boxResUrl(const QString& boxID)
{
    qDebug() << "boxresUrl = " << boxRes(boxID).resourceUri().toString();
    return boxRes(boxID).resourceUri().toString();
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