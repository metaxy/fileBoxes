/*
fileboxes
Copyright (C) 2009  paul wlager

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "mainwindow.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>
#include <KIcon>
static const char description[] =
    I18N_NOOP("A fileboxes example");

static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData aboutData("app_fileboxes", 0,
                         ki18n("Fileboxes Example"), "0.1",
                         ki18n("A fileboxes example"),
                         KAboutData::License_GPL_V3,
                         ki18n("Copyright (c) 2009 Paul Walger"));
    KCmdLineArgs::init(argc, argv, &aboutData);

    KApplication app;

    MainWindow* window = new MainWindow();
    window->setWindowIcon(KIcon("filebox"));
    window->show();

    return app.exec();
}
