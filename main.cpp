#include <QApplication>
#include <QMessageBox>
#include <cstdio>
#include "MainWindow.h"


static void msgHandler(QtMsgType type, char const *message)
{
    fprintf(stderr, "%s\n", message);

    switch (type)
    {
    case QtDebugMsg:
        break;
    case QtWarningMsg:
//        QMessageBox::warning(0, "Warning", message);
        break;
    case QtCriticalMsg:
        QMessageBox::critical(0, "Critical error", message);
        break;
    case QtFatalMsg:
        QMessageBox::critical(0, "Fatal error", message);
        break;
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Xinutec");
    a.setApplicationName("GVoc");

    qInstallMsgHandler(msgHandler);

    MainWindow w;
    w.show();
    
    return a.exec();
}
