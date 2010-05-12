#include "filebox.h"
#include "fileboxicon.h"
#include <plasma/widgets/label.h>
FileBox::FileBox(QGraphicsLinearLayout *parent, const QString boxID, const QString name, const QString icon, BoxesBackend* backend) : QGraphicsWidget()
{
    m_boxID = boxID;
    m_name = name;
    m_icon = icon;
    m_backend = backend;
    m_layout = new QGraphicsLinearLayout(this);
    m_layout->setOrientation(Qt::Horizontal);
    m_layout->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    //_______________________________________
    FileBoxIcon *toolButton = new FileBoxIcon(boxID,name,icon);
    toolButton->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    m_layout->addItem(toolButton);
   
    Plasma::Label *label = new Plasma::Label();
    label->setText(m_name);
    m_layout->addItem(label);
    
}
