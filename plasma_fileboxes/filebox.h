#ifndef FILEBOX_H
#define FILEBOX_H
#include <fileboxes/boxesbackend.h>
 #include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
class FileBox : public QGraphicsWidget
{
public:
    FileBox(QGraphicsLinearLayout *parent,const QString boxID, const QString name, const QString icon, BoxesBackend* backend);
private:
    QString m_boxID;
    QString m_name;
    QString m_icon;
    BoxesBackend* m_backend;
    QGraphicsLinearLayout* m_layout;
};

#endif // FILEBOX_H
