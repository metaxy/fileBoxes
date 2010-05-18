// Here we avoid loading the header multiple times
#ifndef PLASMAFILEBOXES_HEADER
#define PLASMAFILEBOXES_HEADER
// We need the Plasma Applet headers
#include <KIcon>
#include <fileboxes/box.h>
#include <fileboxes/boxesbackend.h>
#include <Plasma/Applet>
#include <Plasma/Svg>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>
#include <QPointF>
#include "ui_fileBoxesConfig.h"
#include "filebox.h"
#include <fileboxes/boxesbackend.h>
#include <KConfigDialog>
#include <kdirwatch.h>
class QSizeF;

// Define our plasma Applet
class PlasmaFileBoxes : public Plasma::Applet
{
    Q_OBJECT
public:
    PlasmaFileBoxes(QObject *parent, const QVariantList &args);
    ~PlasmaFileBoxes();
    void init();
    //void paintInterface(QPainter *painter,const QStyleOptionGraphicsItem *option,const QRect& contentsRect);
protected:
    //void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    //void dropEvent(QGraphicsSceneDragDropEvent *event);
    //virtual QList<QAction*> contextualActions();
    void createConfigurationInterface(KConfigDialog *parent);
    // virtual QList<QAction*> contextualActions();
private:
    QList<QAction *> actions;
    QList<QAction *> boxActions;
    Ui::fileBoxesConfig configUi;
    void createMenu();
    void newBox(QString boxID, QString name, QString icon);

    BoxesBackend *m_backend;
    QGraphicsLinearLayout *m_layout;
    QList<FileBox*> boxes;
    int m_layoutOrientation;
    bool m_showName;
    void loadBoxes();

public slots:
    void newBoxDialog();
    void configAccepted();
    void load();
    void reloadBoxes();
    void slotFilesAdded(QString d);
    void slotFilesRemoved(QStringList fileList);
    void removeBox(QString id);

};

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(plasma - fileboxes, PlasmaFileBoxes)
#endif
