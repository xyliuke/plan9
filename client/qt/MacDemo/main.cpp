#include <QtDebug>
#include <QApplication>
//#include <QWebInspector>

#include "webui.h"
#include "biz_proxy.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    plan9_ui::WebUIWidget w;
    w.setLocalFile("../../../../html/index.html");
    w.show();

//    plan9_ui::biz_proxy::init("", "");

//    {
//        std::shared_ptr<QString> s(new QString("dddd"));
//        qDebug() << s.get();
//        std::weak_ptr<QString> w = s.get();
//    }



    return a.exec();

}
