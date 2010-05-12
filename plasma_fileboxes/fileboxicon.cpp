#include "fileboxicon.h"
#include <QPainter>
#include <QtDebug>
#include <QToolButton>
FileBoxIcon::FileBoxIcon(const QString &boxID, const QString &name, const QString &icon) : Plasma::ToolButton()
{
    this->nativeWidget()->setAcceptDrops(true);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);
    setAcceptsHoverEvents(true);
    setAutoRaise(true);

    m_box =  new Box(boxID, name, icon);
    m_icon = icon;
    setFileBoxIcon(m_icon);

}
void FileBoxIcon::setFileBoxIcon(const QString &iconName)
{
    QColor noNew(69, 147, 219, 255);//blue
    QColor areNew(102, 219, 67, 255);//green
    qreal size = qMin(this->size().width() , this->size().height()) - 5;
    qreal sizeX = size, sizeY = size;
    this->nativeWidget()->setIconSize(QSize(sizeX, sizeY));
    qreal circleWidth = 14, circleHeight = 14;

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
    if (m_box->hasNew()) {
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
    p.drawText(circle, Qt::AlignCenter, QString::number(m_box->size()));

    setIcon(pm);
}
void FileBoxIcon::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    setFileBoxIcon(m_icon);
    Plasma::ToolButton::resizeEvent(event);
}
void FileBoxIcon::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    qDebug() << Q_FUNC_INFO << acceptDrops();
    if (event->mimeData()->hasUrls()) {
        event->accept();
        //event->acceptProposedAction();
    }
}
void FileBoxIcon::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
   event->accept();
}

void FileBoxIcon::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    if (event->mimeData()->hasUrls()) {
        QList<QString> files;
        for (int i = 0; i < event->mimeData()->urls().size(); ++i) {
            files << event->mimeData()->urls().at(i).toLocalFile();
        }
        if (files.size() > 0) {
            m_box->addFiles(files);
            m_box->setHasNew(true);
            setFileBoxIcon(m_icon);
        }
    }
    event->acceptProposedAction();
}

void FileBoxIcon::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << Q_FUNC_INFO;
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
    }
}

void FileBoxIcon::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    QDrag *drag = new QDrag(this->widget());
    QMimeData *mimeData = new QMimeData;

    mimeData->setUrls(m_box->getFiles());
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction);
    m_box->setHasNew(false);
    setFileBoxIcon(m_icon);
}