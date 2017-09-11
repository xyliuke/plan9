#include "webbridge.h"
#include <QDebug>
#include "webui.h"
#include <util/UUID.h>

namespace plan9_ui {

WebBridge* WebBridge::instance()
{
    static WebBridge* wb = nullptr;
    if (wb == nullptr) {
        wb = new WebBridge();
    }
    return wb;
}

WebBridge::WebBridge() : ui_map(new QMap<int, std::shared_ptr<WebUIWidget>>)
{
}

void WebBridge::registerWebWidget(int id, WebUIWidget *widget)
{
    auto p = std::shared_ptr<WebUIWidget>(widget);
    this->ui_map->insert(id, p);
}

void WebBridge::call(int window_id, const QString& event_id, const QString& cmd, const QString& param, bool callback)
{
    qDebug() << "js -> c++ , window id : " << window_id << " , event id : " << event_id << " , cmd : " << cmd << " , param : " << param << " , is callback : " << callback << "\n";
    if (callback) {
        this->callback(window_id, event_id, QString("helloworld"));
    }
}

void WebBridge::callback(int window_id, const QString& event_id, const QString &result)
{
    QString data = QString("__callback__('%1', '%2');").arg(event_id).arg(result);
    qDebug() << "callback data : " << data;
    callJavascript(window_id, data);
}

void WebBridge::notify(const QString &event, const QString &data)
{
    QString str("__notify__('");
    str.append(event);
    str.append("','");
    str.append(data);
    str.append("')");
    qDebug() << "send to js :"<< str;
    callJavascript(str);
}

void WebBridge::callJavascript(int id, const QString &js)
{
    QMap<int, std::shared_ptr<WebUIWidget>>::const_iterator it = this->ui_map->find(id);
    if (it != this->ui_map->end()) {
        auto ui = it.value();
        ui->callJavascript(js);
    }
}

void WebBridge::callJavascript(const QString &js)
{
    QMap<int, std::shared_ptr<WebUIWidget>>::const_iterator it = this->ui_map->begin();
    while (it != this->ui_map->end()) {
        auto ui = it.value();
        ui->callJavascript(js);
        it ++;
    }
}

QString WebBridge::getID()
{
    return QString(plan9::UUID::id().c_str());
}

}


