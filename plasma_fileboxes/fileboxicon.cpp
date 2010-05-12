#include "fileboxicon.h"
#include <QPainter>
#include <QtDebug>
#include <QToolButton>
FileBoxIcon::FileBoxIcon(const QString &boxID, const QString &name, const QString &icon) : Plasma::ToolButton()
{
    setAcceptDrops(true);
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
    this->nativeWidget()->setIconSize(QSize(sizeX,sizeY));
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
