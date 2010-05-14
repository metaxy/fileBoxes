#include "filebox.h"

#include <plasma/widgets/label.h>

FileBox::FileBox(QGraphicsLinearLayout *parent, const QString boxID, const QString name, const QString icon, BoxesBackend* backend) : QGraphicsWidget()
{
    setAcceptDrops(true);
    setAcceptHoverEvents(true);
    setAcceptsHoverEvents(true);
    m_boxID = boxID;
    m_name = name;
    m_icon = icon;
    m_backend = backend;
    m_box = new Box(boxID, name, icon);
    m_layout = new QGraphicsLinearLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    if(parent->orientation() == Qt::Vertical)
        m_layout->setOrientation(Qt::Horizontal);
    else
        m_layout->setOrientation(Qt::Vertical);
    m_layout->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    //_______________________________________
    m_fileBoxIcon = new FileBoxIcon(boxID, name, icon);
    connect(m_fileBoxIcon,SIGNAL(removeB()),this,SLOT(removeBox()));
    m_fileBoxIcon->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_layout->addItem(m_fileBoxIcon);

    Plasma::Label *label = new Plasma::Label();
    label->setText(m_name);
    m_layout->addItem(label);

}

void FileBox::removeBox()
{
    delete this;
}

#include "filebox.moc"