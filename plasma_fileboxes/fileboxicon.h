#ifndef FILEBOXICON_H
#define FILEBOXICON_H
#include <plasma/widgets/toolbutton.h>
#include <QGraphicsLinearLayout>
#include <fileboxes/box.h>
#include <fileboxes/boxesbackend.h>
#include <QGraphicsSceneMouseEvent>
class FileBoxIcon : public Plasma::ToolButton
{
public:
    FileBoxIcon(const QString& boxID, const QString& name, const QString& icon);
    void resizeEvent(QGraphicsSceneResizeEvent *event);
protected:
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
     void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    
private:
    Box *m_box;
    QString m_icon;
    void setFileBoxIcon(const QString &icon);
    QPointF dragStartPosition;
};

#endif // FILEBOXICON_H
