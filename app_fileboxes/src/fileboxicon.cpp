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
#include "fileboxicon.h"
#include <KDE/KIcon>
#include <QtCore/QSize>
#include <QtCore/QUrl>
#include <QtGui/QApplication>
#include <KDE/KIconLoader>
#include <QtGui/QMenu>
#include <QtCore/QtDebug>
#include <QtGui/QIcon>
#include <QtGui/QPen>
#include <QtGui/QPainter>

FileBoxIcon::FileBoxIcon(QWidget *parent, const QString &boxID, const QString &name, const QString &icon)
        : QToolButton(parent)
{
    setAcceptDrops(true);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setAutoRaise(true);
    //setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    backend = new BoxesBackend();
    m_box = new Box(boxID, name, icon);
    setFileBoxID(boxID);
    setFileBoxIcon(icon);
    setFileBoxName(name);
}

FileBoxIcon::~FileBoxIcon()
{
    delete backend;
    delete m_box;
}
void FileBoxIcon::setFileBoxID(const QString &boxID)
{
    this->setObjectName("box_" + boxID);
    this->boxID = boxID;
}
void FileBoxIcon::updateIcon()
{
    setFileBoxIcon(icon);
}
void FileBoxIcon::setFileBoxIcon(const QString &iconName)
{
    QColor noNew(69, 147, 219, 255);//blue
    QColor areNew(102, 219, 67, 255);//green

    unsigned int sizeX = 64, sizeY = 64;
    unsigned int circleWidth = 14, circleHeight = 14;

    QIcon icon = KIcon(iconName);
    QPixmap pm = icon.pixmap(sizeX, sizeY);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);


    QRect circle(pm.width() - circleWidth*1.5, pm.height() - circleHeight*1.5, circleWidth, circleHeight);
    QRect shadowCircle(circle);
    shadowCircle.setX(shadowCircle.x() + 2);
    shadowCircle.setY(shadowCircle.y() + 2);
    //draw shadow
    p.setBrush(QColor(0, 0, 0));
    p.setPen(QColor(0, 0, 0));
    p.drawEllipse(shadowCircle);

    //draw circle
    QPen circlePen;
    if (backend->hasNew(boxID)) {
        circlePen.setColor(areNew);
        p.setBrush(areNew);
    } else {
        circlePen.setColor(noNew);
        p.setBrush(noNew);
    }
    p.setPen(circlePen);
    p.drawEllipse(circle);
    //draw text
    QPen textPen(Qt::black, 1);
    p.setPen(textPen);
    p.setBrush(Qt::NoBrush);
    p.drawText(circle, Qt::AlignCenter, QString::number(backend->boxSize(boxID)));

    setIcon(pm);
    setIconSize(QSize(sizeX, sizeY));
    this->icon = iconName;
}
void FileBoxIcon::setFileBoxName(const QString &name)
{
    this->name = name;
}
void FileBoxIcon::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}
void FileBoxIcon::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QString> files;
        for (int i = 0; i < event->mimeData()->urls().size(); ++i) {
            files << event->mimeData()->urls().at(i).toLocalFile();
        }
        if (files.size() > 0) {
            m_box->addFiles(files);
            m_box->setHasNew(true);
            updateIcon();
        }
    }

    event->acceptProposedAction();
}

void FileBoxIcon::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
    }
}

void FileBoxIcon::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }
    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setUrls(m_box->getFiles());
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction);
    m_box->setHasNew(false);
    updateIcon();
}


