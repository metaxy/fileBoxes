#include <iostream>
#include <QString>
#include <QStringList>
#include <fileboxes/boxesbackend.h>
#include <fileboxes/box.h>
int main(int argc, char *argv[])
{
    if(argc > 1) {
        QString c(argv[1]);
        if(c == "--add-files" && argc > 3) {
            QStringList list;
            QString boxID(argv[2]);
            Box m_box(boxID);
            for(int count = 3; count < argc; count++)  {
                QString s(argv[count]);
                list << s;
            }
            m_box.addFiles(list);

        } else if(c == "--add-box" && argc > 3) {
            QString name(argv[2]);
            BoxesBackend m_backend;
            if(argc == 4) {
                QString icon(argv[3]);
                m_backend.newBox(name, icon);
            } else  {
                m_backend.newBox(name, "filebox");
            }


        } else if(c == "--add-files-in-new-box" && argc > 2) {
            BoxesBackend m_backend;
            QString boxID = m_backend.newBox("", "filebox");
            QStringList list;
            Box m_box(boxID);
            for(int count = 3; count < argc; count++)  {
                QString s(argv[count]);
                list << s;
            }
            m_box.addFiles(list);

        }


    }

    return 0;
}
