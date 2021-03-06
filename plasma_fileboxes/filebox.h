#ifndef FILEBOX_H
#define FILEBOX_H
#include <fileboxes/boxesbackend.h>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneMouseEvent>
#include "fileboxicon.h"
class FileBox : public QGraphicsWidget
{
    Q_OBJECT
public slots:
    void removeBox();
signals:
    void removeMe(QString id);
public:
    FileBox(QGraphicsLinearLayout *parent, const QString boxID, const QString name, const QString icon, BoxesBackend* backend, bool showName);
    FileBoxIcon *m_fileBoxIcon;
    const QString boxID();
protected:


private:
    QString m_boxID;
    QString m_name;
    QString m_icon;
    BoxesBackend* m_backend;
    Box *m_box;
    QGraphicsLinearLayout* m_layout;
    QPoint dragStartPosition;

};

#endif // FILEBOX_H
