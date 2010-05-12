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

#ifndef FILEBOXICON_H
#define FILEBOXICON_H
#include <QtGui/QToolButton>
#include <QtGui/QWidget>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QToolButton>
#include <fileboxes/boxesbackend.h>
#include <fileboxes/box.h>
/**
 * This is a toolButton for the boxes
 *
 * @short QToolButton for boxes
 * @author Paul Walger metaxy@walger.name
 * @version %{VERSION}
 */
class FileBoxIcon  : public QToolButton
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */
    FileBoxIcon(QWidget* parent, const QString& boxID, const QString& name, const QString& icon, BoxesBackend* backend);

    /**
     * Destructor
     */
    virtual ~FileBoxIcon();
    void virtual dropEvent(QDropEvent *event);
    void virtual dragEnterEvent(QDragEnterEvent *event);
    void virtual mouseMoveEvent(QMouseEvent *event);
    void virtual mousePressEvent(QMouseEvent *event);
    void setFileBoxID(const QString &boxID);
    void setFileBoxIcon(const QString &icon);
    void setFileBoxName(const QString &name);
    void updateIcon();

private:
    QString boxID, icon, name;
    Box *m_box;
    BoxesBackend *backend;
    QPoint dragStartPosition;
    BoxesBackend* m_backend;
};

#endif // FILEBOXICON_H
