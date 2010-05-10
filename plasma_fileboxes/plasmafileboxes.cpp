#include "plasmafileboxes.h"
#include <fileboxes/box.h>
#include <fileboxes/boxesbackend.h>
#include <QPainter>
#include "../gui/fileboxeswidget.h"
#include "qgraphicswidgetitem.h"
#include <QFontMetrics>
#include <QSizeF>
#include <QPointF>
#include <KLocale>
#include <QtDebug>
#include <plasma/svg.h>
#include <plasma/theme.h>
#include <QDrag>
#include <KIcon>
#include <KIconLoader>
#include <QGraphicsSceneDragDropEvent>
#include <KMenu>
#include <QProcess>
#include <QInputDialog>
#include <QLineEdit>
#include <QDir>
#include <QDBusInterface>
#include <KConfigDialog>
#include <QStringList>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
plasmaFileBoxes::plasmaFileBoxes(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_svg(this)
{
	setBackgroundHints(DefaultBackground);
	m_svg.setImagePath("");
	setHasConfigurationInterface(true);
	setAspectRatioMode( Plasma::IgnoreAspectRatio );
	setAcceptDrops(true);
	resize(200, 200);

}	


plasmaFileBoxes::~plasmaFileBoxes()
{
}

void plasmaFileBoxes::init()
{


	initBoxes();//
	generateIcons();
	//
	paddingLeft = 17;
	paddingTop = 5;
	KConfigGroup cg = config();
	m_layout = cg.readEntry("layout", 0); // 0 = Vertical, 1 = Horizontal
	m_showName = cg.readEntry("showName", true);
	
	//actions
	actions.clear();
	QAction *anewBox = new QAction(tr("New Box"), this );
	anewBox->setIcon( KIconLoader::global()->loadIcon( "list-add",KIconLoader::NoGroup,48, KIconLoader::DefaultState, QStringList(), 0L, false ) );
	actions.append( anewBox );
	connect( anewBox, SIGNAL( triggered(bool) ), this, SLOT( newBox() ) );
	
	//boxActions
	boxActions.clear();
	//
	QAction *aopenBox = new QAction(tr("Open"), this );
	aopenBox->setIcon( KIconLoader::global()->loadIcon( "system-file-manager",KIconLoader::NoGroup,48, KIconLoader::DefaultState, QStringList(), 0L, false ) );
	boxActions.append( aopenBox );
	connect( aopenBox, SIGNAL( triggered(bool) ), this, SLOT( openBox() ) );
	//
	QAction *aclearBox = new QAction(tr("Clear","Clean up"), this );
	aclearBox->setIcon( KIconLoader::global()->loadIcon( "edit-clear",KIconLoader::NoGroup,48, KIconLoader::DefaultState, QStringList(), 0L, false ) );
	boxActions.append( aclearBox );
	connect( aclearBox, SIGNAL( triggered(bool) ), this, SLOT( clearBox() ) );
	//
	boxActions.append( anewBox );
	//
	QAction *aremoveBox = new QAction(tr("Remove"), this );
	aremoveBox->setIcon( KIconLoader::global()->loadIcon( "list-remove",KIconLoader::NoGroup,48, KIconLoader::DefaultState, QStringList(), 0L, false ) );
	boxActions.append( aremoveBox );
	connect( aremoveBox, SIGNAL( triggered(bool) ), this, SLOT( removeBox() ) );
	//
	
}
void plasmaFileBoxes::createConfigurationInterface(KConfigDialog *parent)
{
	QWidget *widget = new QWidget(parent);
	configUi.setupUi(widget);
	parent->addPage(widget, i18n("General"), Applet::icon());
	connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
	connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
	
	QStringList layouts;
	layouts << "Vertical" << "Horizontal";
	configUi.comboBox_layout->clear();
	configUi.comboBox_layout->insertItems(0,layouts);
	configUi.comboBox_layout->setCurrentIndex(m_layout);
	
	configUi.checkBox_showName->setChecked(m_showName);

}
void plasmaFileBoxes::configAccepted()
{
	bool changed = false;
	KConfigGroup cg = config();
	if (m_showName != configUi.checkBox_showName->isChecked()) 
	{
		m_showName = configUi.checkBox_showName->isChecked();
		cg.writeEntry("showName", m_showName);
		changed = true;
	}
	if (m_layout != configUi.comboBox_layout->currentIndex()) 
	{
		m_layout = configUi.comboBox_layout->currentIndex();
		cg.writeEntry("layout", m_layout);
		changed = true;
	}
	if(changed == true)
	{
		update();
		emit configNeedsSaving();
	}
	
}
void plasmaFileBoxes::paintInterface(QPainter *p,const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
	Q_UNUSED(option);
	p->setRenderHint(QPainter::SmoothPixmapTransform);
	p->setRenderHint(QPainter::Antialiasing);
	m_svg.resize((int)contentsRect.width(), (int)contentsRect.height());
	m_svg.paint(p, (int)contentsRect.left(), (int)contentsRect.top());
	if(icons.size() == 0)
		return;
	if(m_layout == 0)
	{
		int iconHeight = qMin((int)contentsRect.height() / icons.size(),(int)contentsRect.width());
		int iconWidth = iconHeight;
		boxPlaces.clear();
		for(int i=0;i<icons.size();i++)
		{
			QRectF place(paddingLeft,(i*iconHeight)+10,iconWidth,iconHeight);
			boxPlaces << place;
			p->drawPixmap(paddingLeft, (i*iconHeight)+10, icons.at(i).pixmap(iconWidth,iconHeight));
			if(m_showName == true)
			{
				p->setPen(Qt::black);
				p->drawText(QPointF(paddingLeft+iconWidth+10,(i*iconHeight)+10+(iconHeight/2)),boxes.at(i)->name());
			}
			
			//draw shadow
			p->setPen(QColor(0,0,0,100));//black
			p->setBrush(QColor(0,0,0,100));//black
			p->drawEllipse ( QPointF(paddingLeft+iconWidth-13,((i+1)*iconHeight)-3), 8, 8 );
			//draw circle
			QColor noNew(69,147,219,255);
			QColor areNew(102,219,67,255);
			if(boxes.at(i)->hasNew() && boxes.at(i)->size() > 0)
			{
				p->setPen(areNew);
				p->setBrush(areNew);
			}
			else
			{
				p->setPen(noNew);
				p->setBrush(noNew);
			}
			p->drawEllipse ( QPointF(paddingLeft+iconWidth-14,((i+1)*iconHeight)-4), 8, 8 );
			//draw count of files
			p->setPen(Qt::white);
			p->setBrush(Qt::NoBrush);
			int numb = boxes.at(i)->size();
			if(numb < 10)
			{
				p->drawText(QPointF(paddingLeft+iconWidth-18,((i+1)*iconHeight)),QString::number(numb));
			}
			else if(numb < 100)
			{
				p->drawText(QPointF(paddingLeft+iconWidth-22,((i+1)*iconHeight)),QString::number(numb));
			}
			else
			{
				p->drawText(QPointF(paddingLeft+iconWidth-22,((i+1)*iconHeight)),QString::number(numb));//todo: to many files
			}
			
		}
	}
	else if(m_layout == 1)
	{
		int iconWidth;
		if(m_showName == true)
		{
			iconWidth = qMin((int)contentsRect.width() / icons.size(),(int)contentsRect.height()-38);
		}
		else
		{
			iconWidth = qMin((int)contentsRect.width() / icons.size(),(int)contentsRect.height());
		}
		int iconHeight = iconWidth;
		boxPlaces.clear();
		int paddingBetweenBoxes = 0;
		if(iconWidth*(icons.size()) < ((int)contentsRect.width()-paddingLeft*2))
		{
			paddingBetweenBoxes = ((int)contentsRect.width() -paddingLeft*2 - iconWidth*(icons.size()))/icons.size();
		}
		for(int i=0;i<icons.size();++i)
		{
			QRectF place((i*(iconWidth+paddingBetweenBoxes))+paddingLeft,paddingTop,iconWidth,iconHeight);
			boxPlaces << place;
			p->drawPixmap((i*(iconWidth+paddingBetweenBoxes))+paddingLeft,paddingTop, icons.at(i).pixmap(iconWidth,iconHeight));
			if(m_showName == true)
			{
				p->setPen(Qt::black);
				p->drawText(QRectF((i*(iconWidth+paddingBetweenBoxes))+paddingLeft,paddingTop+iconHeight,iconWidth+paddingBetweenBoxes,(int)contentsRect.height()-iconHeight),Qt::AlignHCenter, boxes.at(i)->name());
			}
			int radius=8;
			//draw shadow
			p->setPen(QColor(0,0,0,100));
			p->setBrush(QColor(0,0,0,100));
			p->drawEllipse ( QPointF(((i+1)*(iconWidth+paddingBetweenBoxes))+paddingLeft-13,paddingTop+iconHeight-(iconHeight/6)), radius, radius );
			//draw circle
			QColor noNew(69,147,219,255);//blue
			QColor areNew(102,219,67,255);//green
			if(boxes.at(i)->hasNew() && boxes.at(i)->size() > 0)
			{
				p->setPen(areNew);
				p->setBrush(areNew);
			}
			else
			{
				p->setPen(noNew);
				p->setBrush(noNew);
			}
			p->drawEllipse ( QPointF(((i+1)*(iconWidth+paddingBetweenBoxes))+paddingLeft-14,paddingTop+iconHeight-(iconHeight/6)-1), radius, radius );
			
			//draw count of files
			p->setPen(Qt::white);
			p->setBrush(Qt::NoBrush);
			int numb = boxes.at(i)->size();
			if(numb < 10)
			{
				p->drawText(QPointF(((i+1)*(iconWidth+paddingBetweenBoxes))+paddingLeft-14-(radius/2),paddingTop+iconHeight-(iconHeight/6)-1+(radius/2)),QString::number(numb));
			}
			else if(numb < 100)
			{
				p->drawText(QPointF(((i+1)*(iconWidth+paddingBetweenBoxes))+paddingLeft-14-radius,paddingTop+iconHeight-(iconHeight/6)-1+(radius/2)),QString::number(numb));
			}
			else
			{
				p->drawText(QPointF(((i+1)*(iconWidth+paddingBetweenBoxes))+paddingLeft-14-radius,paddingTop+iconHeight-(iconHeight/6)-1+(radius/2)),QString::number(numb));//todo: to many files
			}
			
		}
	}
	p->save();
	p->restore();
}



#include "plasmafileboxes.moc"
