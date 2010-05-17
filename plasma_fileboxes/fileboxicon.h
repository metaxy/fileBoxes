#ifndef FILEBOXICON_H
#define FILEBOXICON_H
#include <plasma/widgets/toolbutton.h>
#include <QGraphicsLinearLayout>
#include <fileboxes/box.h>
#include <fileboxes/boxesbackend.h>
#include <QGraphicsSceneMouseEvent>
class FileBoxIcon : public Plasma::ToolButton
{
    Q_OBJECT
public:
    FileBoxIcon(const QString& boxID, const QString& name, const QString& icon);
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void updateIcon(bool reloadSize = false);
protected:
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
public slots:
    void newBox();
    void openBoxInNewTab();
    void openBoxInNewWindow();
    void clearBox();
    void removeBox();
    void createArchive();


signals:
    void newBoxDialog();
    void removeB();

private:
    Box *m_box;
    QString m_icon;
    void setFileBoxIcon(const QString& iconName, bool reloadSize = false);
    QPointF dragStartPosition;
    unsigned int m_size;
};

#endif // FILEBOXICON_H
