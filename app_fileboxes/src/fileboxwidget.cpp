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
#include "fileboxwidget.h"
#include "fileboxicon.h"
#include <klocalizedstring.h>
#include <KIcon>
#include <QtCore/QSize>
#include <QtCore/QUrl>
#include <QApplication>
#include <KIconLoader>
#include <QMenu>
#include <QtDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
FileBoxWidget::FileBoxWidget(QWidget *parent, const QString &boxID, const QString &name, const QString &icon)
        : QWidget(parent)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    setContextMenuPolicy(Qt::CustomContextMenu);
    QHBoxLayout *boxLayout = new QHBoxLayout(this);
    boxLayout->setObjectName("layout_" + boxID);

    m_boxIcon = new FileBoxIcon(this, boxID, name, icon);

    QLabel *label = new QLabel(this);
    label->setText(name);
    label->setObjectName("label_" + boxID);
    boxLayout->addWidget(m_boxIcon);
    boxLayout->addWidget(label);

    m_backend = new BoxesBackend();
    m_box = new Box(boxID, name, icon);
    setFileBoxID(boxID);
    setFileBoxIcon(icon);
    setFileBoxName(name);

}

FileBoxWidget::~FileBoxWidget()
{
    delete m_backend;
    delete m_boxIcon;
    delete m_box;
}
void FileBoxWidget::setFileBoxID(const QString &boxID)
{
    this->m_boxID = boxID;
}
void FileBoxWidget::setFileBoxIcon(const QString &icon)
{
    this->m_icon = icon;
}
void FileBoxWidget::setFileBoxName(const QString &name)
{
    this->m_name = name;

}

void FileBoxWidget::contextMenu(const QPoint & pos)
{
    Q_UNUSED(pos);
    qDebug() << "fileBox::customContextMenuRequested";
    QMenu *menu = new QMenu(this);

    QAction *anewBox = new QAction(i18n("New Box"), this);
    anewBox->setIcon(KIconLoader::global()->loadIcon("list-add", KIconLoader::NoGroup, 48, KIconLoader::DefaultState, QStringList(), 0L, false));
    menu->addAction(anewBox);
    connect(anewBox, SIGNAL(triggered(bool)), this, SLOT(newBox()));

    QAction *aopenBox = new QAction(i18n("Open"), this);
    aopenBox->setIcon(KIconLoader::global()->loadIcon("system-file-manager", KIconLoader::NoGroup, 48, KIconLoader::DefaultState, QStringList(), 0L, false));
    menu->addAction(aopenBox);
    connect(aopenBox, SIGNAL(triggered(bool)), this, SLOT(openBox()));

    QAction *atarBox = new QAction(i18n("Tar"), this);
    atarBox->setIcon(KIconLoader::global()->loadIcon("utilities-file-archiver.png", KIconLoader::NoGroup, 48, KIconLoader::DefaultState, QStringList(), 0L, false));
    menu->addAction(atarBox);
    connect(atarBox, SIGNAL(triggered(bool)), this, SLOT(tarBox()));

    QAction *aclearBox = new QAction(i18nc("Clean up", "Clear"), this);
    aclearBox->setIcon(KIconLoader::global()->loadIcon("edit-clear", KIconLoader::NoGroup, 48, KIconLoader::DefaultState, QStringList(), 0L, false));
    menu->addAction(aclearBox);
    connect(aclearBox, SIGNAL(triggered(bool)), this, SLOT(clearBox()));

    QAction *aremoveBox = new QAction(i18n("Remove"), this);
    aremoveBox->setIcon(KIconLoader::global()->loadIcon("list-remove", KIconLoader::NoGroup, 48, KIconLoader::DefaultState, QStringList(), 0L, false));
    menu->addAction(aremoveBox);
    connect(aremoveBox, SIGNAL(triggered(bool)), this, SLOT(removeBox()));
    menu->show();
    menu->exec(QCursor::pos());

}
void FileBoxWidget::newBox()
{
    emit newBoxDialog();
}
void FileBoxWidget::openBox()
{
    QProcess *myProcess = new QProcess();
    //myProcess->start("kioclient",QStringList() << "exec" << "fileboxes:/"+m_box->name());
    myProcess->start("kioclient", QStringList() << "exec" << "fileboxes:/" + m_box->boxID());
    m_box->setHasNew(false);
    m_boxIcon->updateIcon();

}
void FileBoxWidget::tarBox()
{
    QProcess *myProcess = new QProcess();
    //myProcess->start("kioclient",QStringList() << "exec" << "fileboxes:/"+m_box->name());
    QList<QUrl> files2 = m_box->getFiles();
    QList<QString> files;
    for (int i = 0; i < files2.size(); ++i) {
        files << files2.at(i).toLocalFile();
    }
    myProcess->start("ark", QStringList() << "-c" << "-d" << files);
    m_box->setHasNew(false);
}
void FileBoxWidget::clearBox()
{
    m_box->removeAllFiles();
    m_boxIcon->updateIcon();
}
void FileBoxWidget::removeBox()
{
    m_box->removeBox();
    emit removeBox(m_boxID);
}
#include "fileboxwidget.moc"
