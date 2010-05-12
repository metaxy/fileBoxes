#ifndef FILEBOXICON_H
#define FILEBOXICON_H
#include <plasma/widgets/toolbutton.h>
#include <QGraphicsLinearLayout>
#include <fileboxes/box.h>
#include <fileboxes/boxesbackend.h>
class FileBoxIcon : public Plasma::ToolButton
{
public:
    FileBoxIcon(const QString& boxID, const QString& name, const QString& icon);
    void resizeEvent(QGraphicsSceneResizeEvent *event);
private:
    Box *m_box;
    QString m_icon;
    void setFileBoxIcon(const QString &icon);
};

#endif // FILEBOXICON_H
