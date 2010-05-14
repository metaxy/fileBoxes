#include "plasmafileboxes.h"
#include <fileboxes/box.h>
#include <fileboxes/boxesbackend.h>
#include <QPainter>
#include <QFontMetrics>
#include <KLocale>
#include <QtDebug>
#include <plasma/svg.h>
#include <plasma/theme.h>
#include <plasma/widgets/pushbutton.h>
#include <plasma/widgets/label.h>
#include "filebox.h"
#include <KConfigDialog>
#include <QInputDialog>
#include <kinputdialog.h>
PlasmaFileBoxes::PlasmaFileBoxes(QObject *parent, const QVariantList &args)
        : Plasma::Applet(parent, args)
{
    setBackgroundHints(DefaultBackground);

    setHasConfigurationInterface(true);
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setAcceptDrops(true);
    resize(200, 200);
    qDebug() << "init backend";
    m_backend = new BoxesBackend();

}


PlasmaFileBoxes::~PlasmaFileBoxes()
{
}

void PlasmaFileBoxes::init()
{
    KConfigGroup cg = config();
    m_layoutOrientation = cg.readEntry("layout", 0); // 0 = Vertical, 1 = Horizontal
    m_showName = cg.readEntry("showName", true);
    
    m_layout = new QGraphicsLinearLayout;
    this->setLayout(m_layout);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    if(m_layoutOrientation == 0)
        m_layout->setOrientation(Qt::Vertical);
    else
        m_layout->setOrientation(Qt::Horizontal);

    QStringList boxIDs = m_backend->boxIDs();
    for (int i = 0; i < boxIDs.size(); ++i) {
        newBox(boxIDs.at(i), m_backend->name(boxIDs.at(i)), m_backend->icon(boxIDs.at(i)));
    }
    if (boxIDs.size() == 0) {
        QString newBoxID = m_backend->newBox(i18n("Default"), "filebox");
        newBox(newBoxID, i18n("Default"), "filebox");
    }

}
void PlasmaFileBoxes::newBox(QString boxID, QString name, QString icon)
{
    qDebug() << "new Box";
    FileBox *box =  new FileBox(m_layout, boxID, name, icon, m_backend);
    box->setContentsMargins(0,0,0,0);
    connect(box->m_fileBoxIcon,SIGNAL(newBoxDialog()),this,SLOT(newBoxDialog()));
    box->setObjectName("box_" + boxID);
    box->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    m_layout->addItem(box);
    
  
   

}

void PlasmaFileBoxes::newBoxDialog()
{
    qDebug() << "newBoxDialog";
    bool ok;
    QString boxName;
    
    QString text = KInputDialog::getText(i18n("Box Name"), i18n("Box Name:"), i18n("Box"), &ok);
    if (ok && !text.isEmpty()) {
        boxName = text;
    } else {
        boxName = i18n("Box");
    }
    QString boxID = m_backend->newBox(boxName, "filebox");
    newBox(boxID, boxName, "filebox");
}

/*  initBoxes();//
    generateIcons();
    //
    paddingLeft = 17;
    paddingTop = 5;
  

//
*/

void PlasmaFileBoxes::createConfigurationInterface(KConfigDialog *parent)
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
    configUi.comboBox_layout->setCurrentIndex(m_layoutOrientation);

    configUi.checkBox_showName->setChecked(m_showName);

}
void PlasmaFileBoxes::configAccepted()
{
    bool changed = false;
    KConfigGroup cg = config();
    if (m_showName != configUi.checkBox_showName->isChecked())
    {
        m_showName = configUi.checkBox_showName->isChecked();
        cg.writeEntry("showName", m_showName);
        changed = true;
    }
    if (m_layoutOrientation != configUi.comboBox_layout->currentIndex())
    {
        m_layoutOrientation = configUi.comboBox_layout->currentIndex();
        cg.writeEntry("layout", m_layoutOrientation);
        changed = true;
    }
    if(changed == true)
    {
        if(m_layoutOrientation == 0)
            m_layout->setOrientation(Qt::Vertical);
        else
            m_layout->setOrientation(Qt::Horizontal);
        emit configNeedsSaving();
    }

}
/*void PlasmaFileBoxes::paintInterface(QPainter *p,const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
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
}*/



#include "plasmafileboxes.moc"
