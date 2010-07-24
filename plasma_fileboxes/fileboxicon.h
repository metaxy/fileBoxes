#ifndef FILEBOXICON_H
#define FILEBOXICON_H
#include <plasma/widgets/toolbutton.h>
#include <QGraphicsLinearLayout>
#include <fileboxes/box.h>
#include <fileboxes/boxesbackend.h>
#include <QGraphicsSceneMouseEvent>
#include <plasma/widgets/iconwidget.h>
class FileBoxIcon : public Plasma::IconWidget
{
    Q_OBJECT
public:
    FileBoxIcon(QGraphicsItem *parent, const QString& boxID, const QString& name, const QString& icon);
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
    void openDialog();

signals:
    void newBoxDialog();
    void removeB();

private:
    Box *m_box;
    QString m_icon;
    void setFileBoxIcon(const QString& iconName, bool reloadSize = false);
    QPointF dragStartPosition;
    unsigned int m_size;
    QString m_name;

};

#endif // FILEBOXICON_H
