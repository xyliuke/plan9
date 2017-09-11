#ifndef WEBBRIDGE_H
#define WEBBRIDGE_H

#include <QObject>

namespace plan9_ui {

class WebUIWidget;

class WebBridge : public QObject
{
    Q_OBJECT
public:
    static WebBridge* instance();

    void registerWebWidget(int id, WebUIWidget* widget);

    void callJavascript(int id, const QString& js);
    void callJavascript(const QString& js);

    void callback(int window_id, const QString& event_id, const QString &result);
    void notify(const QString& event, const QString& data);

public slots:
    void call(int window_id, const QString& event_id, const QString& cmd, const QString& param, bool callback);
    QString getID();

private:
    explicit WebBridge();
    std::shared_ptr<QMap<int, std::shared_ptr<WebUIWidget>>> ui_map;
};

}

#endif // WEBBRIDGE_H
