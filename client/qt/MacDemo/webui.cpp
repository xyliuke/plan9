#include "webui.h"
#include <QUrl>
#include <QHBoxLayout>
#include <QDir>
#include <QWebFrame>
#include "webbridge.h"

namespace plan9_ui {

static int get_ui_id()
{
    static int id = 0;
    return id;
}

WebUIWidget::WebUIWidget() : web(new QWebView) , inspector(new QWebInspector)
{
    id = get_ui_id();
    QLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);

    this->web->page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    this->web->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    layout->addWidget(this->web.get());

    QObject* sender = (QObject*)this->web->page()->mainFrame();
    connect(sender, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(populateJavaScriptWindowsObject()));

    connect(sender, SIGNAL(loadFinished(bool)), this, SLOT(loadFinish(bool)));

    WebBridge::instance()->registerWebWidget(id, this);

    inspector->setPage(this->web->page());
}

WebUIWidget::~WebUIWidget()
{
}

void WebUIWidget::setUrl(QString url)
{
    QUrl u(url);
    this->web->setUrl(u);
}

void WebUIWidget::setLocalFile(QString file)
{
    QDir dir;
    QString path = dir.currentPath();
    QUrl u = QUrl::fromLocalFile(path.append(file));
    this->web->setUrl(u);
    qDebug() << "local file : " << u;
}

void WebUIWidget::populateJavaScriptWindowsObject() {
    this->web->page()->mainFrame()->addToJavaScriptWindowObject("__js_caller__", WebBridge::instance());
}

void WebUIWidget::loadFinish(bool load)
{
    if(load) {
        set_web_window_id();
    }
}

void WebUIWidget::set_web_window_id()
{
    QString str = QString("__set_window_id__(%1);").arg(id);
    qDebug() << str;
    callJavascript(str);
}

void WebUIWidget::callJavascript(const QString &js)
{
    this->web->page()->mainFrame()->evaluateJavaScript(js);
}

void WebUIWidget::showWebInspector(bool show)
{
    inspector->setVisible(show);
}

void WebUIWidget::setFramelessWindow(bool frameless)
{
    if (frameless) {
        setWindowFlags(Qt::FramelessWindowHint);
    } else {
        setWindowFlags(Qt::Window);
    }
}

}



