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
    //setAcceptDrops(true);
    resize(200, 200);
    m_backend = new BoxesBackend();

}


PlasmaFileBoxes::~PlasmaFileBoxes()
{
    delete m_backend;
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
    m_layout->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    loadBoxes();
}
void PlasmaFileBoxes::loadBoxes()
{
    qDebug() << "laod";

    if (m_layoutOrientation == 0)
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
    FileBox *box =  new FileBox(m_layout, boxID, name, icon, m_backend, m_showName);
    box->setContentsMargins(0, 0, 0, 0);
    connect(box->m_fileBoxIcon, SIGNAL(newBoxDialog()), this, SLOT(newBoxDialog()));
    box->setObjectName("box_" + boxID);
    box->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_layout->addItem(box);
    boxes << box;




}

void PlasmaFileBoxes::newBoxDialog()
{
    qDebug() << "newBoxDialog";
    bool ok;
    QString boxName;
    QString name = i18n("Box");
    if (m_backend->boxIDs().contains(name)) {
        for (int i = 1;; i++) {
            QString n(name + " (" + QString::number(i) + ")");
            if (!m_backend->boxIDs().contains(n)) {
                name = n;
                break;
            }
        }
    }
    QString text = KInputDialog::getText(i18n("Box Name"), i18n("Box Name:"), name, &ok);
    if (ok && !text.isEmpty()) {
        boxName = text;
    } else {
        QString bID = i18n("Box");
    }

    QString boxID = m_backend->newBox(boxName, "filebox");
    newBox(boxID, boxName, "filebox");
}

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
    configUi.comboBox_layout->insertItems(0, layouts);
    configUi.comboBox_layout->setCurrentIndex(m_layoutOrientation);

    configUi.checkBox_showName->setChecked(m_showName);

}
void PlasmaFileBoxes::configAccepted()
{
    qDebug() << "config Accepted";
    bool changed = false;
    bool reload = false;
    KConfigGroup cg = config();
    if (m_showName != configUi.checkBox_showName->isChecked()) {
        m_showName = configUi.checkBox_showName->isChecked();
        cg.writeEntry("showName", m_showName);
        reload = true;

    }
    if (m_layoutOrientation != configUi.comboBox_layout->currentIndex()) {
        m_layoutOrientation = configUi.comboBox_layout->currentIndex();
        cg.writeEntry("layout", m_layoutOrientation);
        changed = true;
    }
    if (reload == true) {
        foreach(FileBox *box, boxes) {
            delete box;
            box = 0;
        }
        boxes.clear();
        loadBoxes();
        emit configNeedsSaving();
    } else if (changed == true) {
        if (m_layoutOrientation == 0)
            m_layout->setOrientation(Qt::Vertical);
        else
            m_layout->setOrientation(Qt::Horizontal);
        emit configNeedsSaving();
    }

}




#include "plasmafileboxes.moc"
