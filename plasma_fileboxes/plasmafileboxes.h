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
class QSizeF;

// Define our plasma Applet
class plasmaFileBoxes : public Plasma::Applet
{
	Q_OBJECT
	public:
		plasmaFileBoxes(QObject *parent, const QVariantList &args);
		~plasmaFileBoxes();
		void init();
		void paintInterface(QPainter *painter,const QStyleOptionGraphicsItem *option,const QRect& contentsRect);
	protected:
		void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
		void dropEvent(QGraphicsSceneDragDropEvent *event);
		virtual QList<QAction*> contextualActions();
		void createConfigurationInterface(KConfigDialog *parent);
	private:
		Plasma::Svg m_svg;
		QList<QAction *> actions;
		QList<QAction *> boxActions;

		Ui::fileBoxesConfig configUi;

		void createMenu();

	public slots:
		void configAccepted();
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(plasmaFileBoxes, plasmaFileBoxes)
#endif
