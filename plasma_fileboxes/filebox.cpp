#include "filebox.h"

#include <plasma/widgets/label.h>

FileBox::FileBox(QGraphicsLinearLayout* parent, const QString boxID, const QString name, const QString icon, BoxesBackend* backend, bool showName) : QGraphicsWidget()
{
    qDebug() << "New File Box";
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
    /*if(parent && parent->orientation()) {
        if(parent->orientation() == Qt::Vertical)
            m_layout->setOrientation(Qt::Horizontal);
        else
            m_layout->setOrientation(Qt::Vertical);
    } else {
        m_layout->setOrientation(Qt::Horizontal);
    }*/

    m_layout->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    //_______________________________________
    m_fileBoxIcon = new FileBoxIcon(this, boxID, name, icon);
    connect(m_fileBoxIcon, SIGNAL(removeB()), this, SLOT(removeBox()));
    m_fileBoxIcon->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
    m_layout->addItem(m_fileBoxIcon);
    m_layout->setAlignment(m_fileBoxIcon, Qt::AlignLeft);
    if(showName) {
        qDebug() << "show Name true";
        Plasma::Label *label = new Plasma::Label();
        label->setText(m_name);
        m_layout->addItem(label);
        m_layout->setAlignment(label, Qt::AlignJustify);
    }

}
const QString FileBox::boxID()
{
    return m_boxID;
}

void FileBox::removeBox()
{
    emit removeMe(m_boxID);
}

#include "filebox.moc"