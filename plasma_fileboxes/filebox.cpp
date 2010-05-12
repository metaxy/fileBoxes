#include "filebox.h"

#include <plasma/widgets/label.h>

FileBox::FileBox(QGraphicsLinearLayout *parent, const QString boxID, const QString name, const QString icon, BoxesBackend* backend) : QGraphicsWidget()
{
    setAcceptDrops(true);
    m_boxID = boxID;
    m_name = name;
    m_icon = icon;
    m_backend = backend;
    m_layout = new QGraphicsLinearLayout(this);
    m_layout->setOrientation(Qt::Horizontal);
    m_layout->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    //_______________________________________
    m_fileBoxIcon = new FileBoxIcon(boxID, name, icon);
    m_fileBoxIcon->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_layout->addItem(m_fileBoxIcon);

    Plasma::Label *label = new Plasma::Label();
    label->setText(m_name);
    m_layout->addItem(label);

}
