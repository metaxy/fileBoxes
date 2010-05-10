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

#ifndef FILEBOXWIDGET_H
#define FILEBOXWIDGET_H
#include <QtGui/QToolButton>
#include <QtGui/QWidget>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtCore/QPoint>
#include <QtGui/QPushButton>
#include <fileboxes/boxesbackend.h>
#include <fileboxes/box.h>
#include "fileboxicon.h"
/**
* This is a toolButton for the boxes
*
* @short QToolButton for boxes
* @author Paul Walger metaxy@walger.name
* @version %{VERSION}
*/
class FileBoxWidget  : public QWidget
{
    Q_OBJECT
public:
    /**
    * Default constructor
    */
    FileBoxWidget(QWidget *parent, const QString &boxID, const QString &name, const QString &icon);

    /**
    * Destructor
    */
    virtual ~FileBoxWidget();

    void setFileBoxID(const QString &boxID);
    void setFileBoxIcon(const QString &icon);
    void setFileBoxName(const QString &name);
public slots:
    void contextMenu(const QPoint & pos);
    void newBox();
    void openBox();
    void clearBox();
    void removeBox();
    void tarBox();
signals:
    void newBoxDialog();
    void removeBox(QString boxID);

private:
    QString m_boxID;
    QString m_icon;
    QString m_name;
    Box *m_box;
    FileBoxIcon *m_boxIcon;
    BoxesBackend *m_backend;
};

#endif // FILEBOXWIDGET_H
