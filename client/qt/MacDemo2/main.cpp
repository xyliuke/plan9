#include "basewebwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BaseWebWidget b;
    b.setUrl(QString("http://www.cn.bing.com"));
    b.show();
    return a.exec();
}
