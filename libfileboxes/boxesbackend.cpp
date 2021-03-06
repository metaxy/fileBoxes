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

#include <Soprano/QueryResultIterator>
#include <Soprano/Vocabulary/NAO>
#include <KDirWatch>
#include <QDir>
#include <kdirnotify.h>
using namespace Nepomuk;
BoxesBackend::BoxesBackend()
{
    m_loaded = Nepomuk::ResourceManager::instance()->init();
    m_model = Nepomuk::ResourceManager::instance()->mainModel();
}
BoxesBackend::~BoxesBackend()
{
}
bool BoxesBackend::newFile(const QString& fileName, const QString& boxID)
{
    qDebug() << Q_FUNC_INFO <<  fileName;
    Resource f(fileName);
    f.addProperty(Nepomuk::Vocabulary::NIE::isPartOf(), boxRes(boxID));
    return true;
}
QString BoxesBackend::newBox(const QString &name, const QString &icon)
{
    QString boxID = name;
    if(name == "") {
        boxID = i18n("Box");
    }


    if(boxIDs().contains(name)) {
        for(int i = 1;; i++) {
            QString n(name + " (" + QString::number(i) + ")");
            if(!boxIDs().contains(n)) {
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

    org::kde::KDirNotify::emitFilesAdded("fileboxes:/");
    return boxID;
}

bool BoxesBackend::removeFile(const QString &fileName, const QString &boxID)
{
    KUrl u(fileName);
    QString n = QString::fromAscii(u.toEncoded().toPercentEncoding(QByteArray(), QByteArray(""), '_'));
    Resource f(fileName);
    org::kde::KDirNotify::emitFilesRemoved(QStringList() << "fileboxes:/" + boxID + "/" + n);

    f.removeProperty(Nepomuk::Vocabulary::NIE::isPartOf(), boxRes(boxID));
    return true;
}
bool BoxesBackend::removeFiles(const QStringList &files, const QString &boxID)
{
    bool ok = true;
    foreach(QString file,files) {
        ok &= removeFile(file,boxID);
    }
    return ok;
}
bool BoxesBackend::removeAllFiles(const QString &boxID)
{
    Nepomuk::Query::ResourceTerm partTerm(boxRes(boxID));
    Nepomuk::Query::ComparisonTerm term(Nepomuk::Vocabulary::NIE::isPartOf(),
                                        partTerm,
                                        Nepomuk::Query::ComparisonTerm::Equal);
    Nepomuk::Query::Query query(term);
    QString q = query.toSparqlQuery();
    Soprano::QueryResultIterator it = m_model->executeQuery(q, Soprano::Query::QueryLanguageSparql);
    while(it.next()) {
        Resource f(it.binding(0).uri());
        f.removeProperty(Nepomuk::Vocabulary::NIE::isPartOf(), boxRes(boxID));
    }
    return true;
}
QList<QUrl> BoxesBackend::files(const QString &boxID)
{
    /*Nepomuk::Query::ResourceTerm partTerm(boxRes(boxID));
    Nepomuk::Query::ComparisonTerm term(Nepomuk::Vocabulary::NIE::isPartOf(),
                                        partTerm,
                                        Nepomuk::Query::ComparisonTerm::Equal);
    Nepomuk::Query::Query query(term);
    QString q = query.toSparqlQuery();
    qDebug() << q;*/
    QString query
       = QString("select distinct ?r where { ?r %1 %2 . }")
       .arg( Soprano::Node::resourceToN3(Nepomuk::Vocabulary::NIE::isPartOf()) )
       .arg( Soprano::Node::resourceToN3(boxRes(boxID).resourceUri()) );
    qDebug() << query;        
    Soprano::QueryResultIterator it = m_model->executeQuery(query, Soprano::Query::QueryLanguageSparql);
    QList<QUrl> urls;
    while(it.next()) {
        qDebug() << it.bindingNames() << it.bindingCount() << it.isBinding() << it.isGraph() << it.isValid();
        Resource f(it.binding(0).uri());
        urls << it.binding(0).uri();
    }
    qDebug() << Q_FUNC_INFO << urls; 
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

    Soprano::QueryResultIterator it = m_model->executeQuery(q, Soprano::Query::QueryLanguageSparql);
    QStringList list;
    while(it.next()) {
        Resource f(it.binding(0).uri());
        list << f.property(Nepomuk::Vocabulary::FB::boxID()).toString();
    }

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

    Soprano::QueryResultIterator it = m_model->executeQuery(q, Soprano::Query::QueryLanguageSparql);
    QStringList list;
    while(it.next()) {
        Resource f(it.binding(0).uri());
        list << f.label();
    }

    return list;
}

QString BoxesBackend::name(const QString &boxID)
{
    Resource boxR = boxRes(boxID);
    QString label = boxR.label();
    return label;
}

QString BoxesBackend::icon(const QString &boxID)
{
    Resource boxR = boxRes(boxID);
    QString icon = boxR.property(Soprano::Vocabulary::NAO::iconName()).toString();
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
const Nepomuk::Resource BoxesBackend::fileBoxesRes() const
{
    QUrl boxesUrl(QDir::homePath());
    Resource res(boxesUrl);
    return res;

}
QString BoxesBackend::boxResUrl(const QString& boxID)
{
    return boxRes(boxID).resourceUri().toString();
}
QString BoxesBackend::localPath(QUrl url)
{
    Resource f(url);
    return f.property(Nepomuk::Vocabulary::NIE::url()).toUrl().toString();
}
QUrl BoxesBackend::localUrl(QUrl url)
{
    Resource f(url);
    return f.property(Nepomuk::Vocabulary::NIE::url()).toUrl();
}