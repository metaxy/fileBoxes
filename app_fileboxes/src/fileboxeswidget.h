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

#ifndef FILEBOXESWIDGET_H
#define FILEBOXESWIDGET_H
#include <QtGui/QWidget>
#include <QtGui/QBoxLayout>
#include <QtGui/QVBoxLayout>
#include <fileboxes/boxesbackend.h>
#include <fileboxes/box.h>
#include "fileboxwidget.h"
class FileBoxesWidget: public QWidget
{
    Q_OBJECT
public:
    /**
    * Default constructor
    */
    FileBoxesWidget(QWidget *parent);
    /**
    * Destructor
    */
    virtual ~FileBoxesWidget();
private:
    QWidget *mainWidget;
    QBoxLayout *mainLayout;
    QVBoxLayout *topLayout;
    void newBox(const QString &id, const QString &name, const QString &icon);

    BoxesBackend *backend;
private slots:
    void removeBox(const QString &boxID);
    void newBoxDialog();
};

#endif // FILEBOXESWIDGET_H
