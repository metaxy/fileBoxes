#include "filebox.h"
#include <plasma/widgets/label.h>
FileBox::FileBox(QGraphicsLinearLayout *parent, const QString boxID, const QString name, const QString icon, BoxesBackend* backend) : QGraphicsWidget()
{
    m_boxID = boxID;
    m_name = name;
    m_icon = icon;
    m_backend = backend;
    m_layout = new QGraphicsLinearLayout(this);
    //_______________________________________
    Plasma::Label *label = new Plasma::Label();
    label->setText(m_name);
    m_layout->addItem(label);
    
}
