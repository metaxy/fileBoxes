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
#include "fileboxeswidget.h"
#include "fileboxwidget.h"
#include "fileboxicon.h"
#include <klocalizedstring.h>
#include <KDE/KIcon>
#include <QtGui/QLabel>
#include <QtGui/QSizePolicy>
#include <QtCore/QtDebug>
#include <QtCore/QStringList>
#include <QtGui/QInputDialog>
FileBoxesWidget::FileBoxesWidget(QWidget *parent)
        : QWidget(parent)
{
    qDebug() << "fileBoxesWidget::fileBoxesWidget";
    setAcceptDrops(true);
    backend = new BoxesBackend();

    mainWidget = new QWidget(this);

    topLayout = new QVBoxLayout(this);
    topLayout->setObjectName("l_layout");
    topLayout->addWidget(mainWidget);


    mainWidget->setObjectName("fileBoxesWidget_mainWidget");
    QStringList boxIDs = backend->boxIDs();
    for (int i = 0; i < boxIDs.size(); ++i) {
        newBox(boxIDs.at(i), backend->name(boxIDs.at(i)), backend->icon(boxIDs.at(i)));
    }
    if (boxIDs.size() == 0) {
        QString newBoxID = backend->newBox(i18n("Default"), "filebox");
        newBox(newBoxID, i18n("Default"), "filebox");
    }

}
FileBoxesWidget::~FileBoxesWidget()
{
    delete backend;
    //delete mainWidget;
    //delete topLayout;
}

void FileBoxesWidget::newBox(const QString &boxID, const QString &name, const QString &icon)
{
    FileBoxWidget *box = new FileBoxWidget(topLayout->widget(), boxID, name, icon);
    connect(box, SIGNAL(removeBox(QString)), this, SLOT(removeBox(QString)));
    connect(box, SIGNAL(newBoxDialog()), this, SLOT(newBoxDialog()));
    box->setObjectName("boxWidget_" + boxID);
    topLayout->addWidget(box);

}
void FileBoxesWidget::removeBox(const QString &boxID)
{
    delete this->findChild<QWidget*>("boxWidget_" + boxID);
}
void FileBoxesWidget::newBoxDialog()
{
    bool ok;
    QString boxName;

    QString text = QInputDialog::getText(this, i18n("Box Name"), i18n("Box Name:"), QLineEdit::Normal, i18n("Box"), &ok);
    if (ok && !text.isEmpty()) {
        boxName = text;
    } else {
        boxName = i18n("Box");
    }
    QString boxID = backend->newBox(boxName, "filebox");
    newBox(boxID, boxName, "filebox");
}

