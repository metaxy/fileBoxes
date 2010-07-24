#include "fileboxicon.h"
#include <fileboxes/box.h>
#include <QPainter>
#include <QtDebug>
#include <QToolButton>
#include <QProcess>
#include <QDBusMessage>
#include <QDBusConnection>
#include <KIconLoader>
#include <KIcon>
#include <QMenu>
#include <QDBusInterface>
#include <klocale.h>
#include <QDomDocument>
#include <taskmanager/task.h>
#include <taskmanager/taskmanager.h>
#include <taskmanager/taskmanager.h>
#include <Plasma/ToolTipManager>
#include <plasma/widgets/iconwidget.h>

FileBoxIcon::FileBoxIcon(QGraphicsItem* parent, const QString& boxID, const QString& name, const QString& icon)
    : Plasma::IconWidget(parent)
{
    // this->nativeWidget()->setAcceptDrops(true);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);
    setAcceptsHoverEvents(true);
//   //  setAutoRaise(true);

    m_box = new Box(boxID, name, icon);
    m_icon = icon;
    m_name = name;
    setFileBoxIcon(m_icon, true);
    connect(this, SIGNAL(clicked()), this, SLOT(openBoxInNewTab()));



}
void FileBoxIcon::updateIcon(bool reloadSize)
{
    setFileBoxIcon(m_icon, reloadSize);
}
void FileBoxIcon::setFileBoxIcon(const QString &iconName, bool reloadSize)
{
    if(reloadSize) {
        m_size = m_box->size();

        QString items = i18n("%1 items", QString::number(m_size));
        Plasma::ToolTipContent d(m_name, items, KIcon(m_icon));
        Plasma::ToolTipManager::self()->setContent(this, d);
    }



    QColor noNew(69, 147, 219, 255);//blue
    QColor areNew(102, 219, 67, 255);//green
    qreal size = qMin(this->size().width() , this->size().height()) - 1;
    qreal sizeX = size, sizeY = size;
    // this->nativeWidget()->setIconSize(QSize(sizeX, sizeY));
    qreal circleWidth = 14, circleHeight = 14;

    QIcon icon = KIcon(iconName);
    QPixmap pm = icon.pixmap(sizeX, sizeY);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);


    QRect circle(pm.width() - circleWidth * 1.5, pm.height() - circleHeight * 1.5, circleWidth, circleHeight);
    QRect shadowCircle(circle);
    shadowCircle.setX(shadowCircle.x() + 2);
    shadowCircle.setY(shadowCircle.y() + 2);
    //draw shadow
    p.setBrush(QColor(0, 0, 0));
    p.setPen(QColor(0, 0, 0));
    p.drawEllipse(shadowCircle);

    //draw circle
    QPen circlePen;
    if(m_box->hasNew() && m_size != 0) {
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

    p.drawText(circle, Qt::AlignCenter, QString::number(m_size));

    setIcon(pm);
}
void FileBoxIcon::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    setFileBoxIcon(m_icon);
    Plasma::IconWidget::resizeEvent(event);
}
void FileBoxIcon::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    if(event->mimeData()->hasUrls()) {
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
    if(event->mimeData()->hasUrls()) {
        QList<QString> files;
        for(int i = 0; i < event->mimeData()->urls().size(); ++i) {
            qDebug() << event->mimeData()->urls().at(i) << event->mimeData()->urls().at(i).toString();
            files << event->mimeData()->urls().at(i).toString();
        }
        if(files.size() > 0) {
            m_box->addFiles(files);
            m_box->setHasNew(true);
            updateIcon(true);
        }
    }
    event->acceptProposedAction();
}

void FileBoxIcon::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
    }
    Plasma::IconWidget::mousePressEvent(event);
}

void FileBoxIcon::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!(event->buttons() & Qt::LeftButton)) {
        return;
    }
    QWidget *w = new QWidget;
    QDrag *drag = new QDrag(w);
    QMimeData *mimeData = new QMimeData;

    mimeData->setUrls(m_box->getFiles());
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction);
    m_box->setHasNew(false);
    updateIcon();
}
void FileBoxIcon::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{

    QMenu *menu = new QMenu;

    QAction *actionOpenBoxInNewTab = new QAction(KIcon("system-file-manager"), i18n("Open in Tab"), this);
    connect(actionOpenBoxInNewTab, SIGNAL(triggered(bool)), this, SLOT(openBoxInNewTab()));

    QAction *actionOpenBoxInNewWindow = new QAction(KIcon("system-file-manager"), i18n("Open in Window"), this);
    connect(actionOpenBoxInNewWindow, SIGNAL(triggered(bool)), this, SLOT(openBoxInNewWindow()));

    QAction *actionCreateArchive = new QAction(KIcon("utilities-file-archiver"), i18n("Create Archive"), this);
    connect(actionCreateArchive, SIGNAL(triggered(bool)), this, SLOT(createArchive()));

    QAction *actionNewBox = new QAction(KIcon("list-add"), i18n("New Box"), this);
    connect(actionNewBox, SIGNAL(triggered(bool)), this, SLOT(newBox()));

    QAction *actionCleanUp = new QAction(KIcon("edit-clear"), i18n("Clean up"), this);
    connect(actionCleanUp, SIGNAL(triggered(bool)), this, SLOT(clearBox()));

    QAction *actionRemove = new QAction(KIcon("list-remove"), i18n("Remove"), this);
    connect(actionRemove, SIGNAL(triggered(bool)), this, SLOT(removeBox()));

    menu->addAction(actionOpenBoxInNewTab);
    menu->addAction(actionOpenBoxInNewWindow);
    menu->addAction(actionCreateArchive);
    menu->addSeparator();
    menu->addAction(actionNewBox);
    menu->addAction(actionCleanUp);
    menu->addAction(actionRemove);

    menu->show();
    menu->exec(QCursor::pos());
}
void FileBoxIcon::newBox()
{
    emit newBoxDialog();
}
void FileBoxIcon::openBoxInNewWindow()
{
    QProcess *myProcess = new QProcess();
    myProcess->start("kioclient", QStringList() << "exec" << "fileboxes:/" + m_box->boxID());
    m_box->setHasNew(false);
    updateIcon();

}
void FileBoxIcon::openBoxInNewTab()
{
    QDBusMessage listOfD = QDBusMessage::createMethodCall("org.kde.dolphin",
                           "/dolphin",
                           "org.freedesktop.DBus.Introspectable",
                           "Introspect");
    QDBusMessage listOfDRes = QDBusConnection::sessionBus().call(listOfD);
    if(listOfDRes.arguments().size() == 0) {
        openBoxInNewWindow();
        return;
    }
    QString xml = listOfDRes.arguments().at(0).toString();
    QString search = "Dolphin_";
    int next = xml.indexOf(search);
    QStringList items;
    while(true) {
        QString backup = xml;
        backup.remove(0, next + search.size());
        backup.remove(backup.indexOf("\""), backup.size());
        items << backup;
        xml.remove(search + backup);
        next = xml.indexOf(search);
        if(next == -1)
            break;
    }
    QString rID;
    QList<QString> windows;
    QMap<QString, TaskManager::Task *> map;
    foreach(QString item, items) {
        QDBusMessage a = QDBusMessage::createMethodCall("org.kde.dolphin",
                         "/dolphin/Dolphin_" + item,
                         "org.kde.KMainWindow",
                         "winId");
        QDBusMessage b = QDBusConnection::sessionBus().call(a);
        if(b.arguments().size() == 0 || b.arguments().at(0).toString() == "")
            continue;
        QString id = b.arguments().at(0).toString();
        TaskManager::Task *task = new TaskManager::Task((WId)id.toLong(), this->parentObject(), "");
        if(task->isOnCurrentDesktop()) {
            windows << item;
            map[item] = task;
        }
    }

    foreach(QString id, windows) {
        TaskManager::Task *task = map.value(id);
        if(task->isActive()) {
            rID = id;
            break;
        }
    }
    if(rID.isEmpty()) {
        foreach(QString id, windows) {
            TaskManager::Task *task = map.value(id);
            if(task->isAlwaysOnTop()) {
                rID = id;
                break;
            }

        }
    }
    if(rID.isEmpty()) {
        foreach(QString id, windows) {
            TaskManager::Task *task = map.value(id);
            if(task->isOnTop()) {
                rID = id;
                break;
            }

        }
    }
    if(rID.isEmpty()) {
        foreach(QString id, windows) {
            TaskManager::Task *task = map.value(id);
            if(!task->isMinimized()) {
                rID = id;
                break;
            }
        }

    }
    if(rID.isEmpty()) {
        foreach(QString id, windows) {
            TaskManager::Task *task = map.value(id);
            rID = id;
            break;
        }

    }
    if(rID.isEmpty()) {
        openBoxInNewWindow();
    } else {
        TaskManager::Task *task = map.value(rID);
        task->activateRaiseOrIconify();
        QDBusMessage m = QDBusMessage::createMethodCall("org.kde.dolphin", "/dolphin/Dolphin_" + rID, "org.kde.KMainWindow", "activateAction");
        m << "new_tab";
        bool queued = QDBusConnection::sessionBus().send(m);

        QDBusMessage m2 = QDBusMessage::createMethodCall("org.kde.dolphin", "/dolphin/Dolphin_" + rID, "org.kde.dolphin.MainWindow", "changeUrl");
        m2 << "fileboxes:/" + m_box->boxID();
        bool queued2 = QDBusConnection::sessionBus().send(m2);

    }
    m_box->setHasNew(false);
    updateIcon();

}
void FileBoxIcon::createArchive()
{
    QProcess *myProcess = new QProcess();
    QList<QUrl> urls = m_box->getFiles();
    QList<QString> files;
    foreach(QUrl url, urls) {
        files << url.toString();
    }
    myProcess->start("ark", QStringList() << "-c" << "-d" << files);
    m_box->setHasNew(false);
    updateIcon();
}
void FileBoxIcon::clearBox()
{
    m_box->removeAllFiles();
    updateIcon(true);
}
void FileBoxIcon::removeBox()
{
    m_box->removeBox();
    emit removeB();
}
void FileBoxIcon::openDialog()
{
}