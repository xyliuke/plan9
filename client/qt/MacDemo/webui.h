#ifndef WEBUI_H
#define WEBUI_H

#include <QWidget>
#include <QWebView>
#include <QWebInspector>

namespace plan9_ui {

class WebUIWidget : public QWidget
{
    Q_OBJECT
public:
    WebUIWidget();
    ~WebUIWidget();

    inline int getID() {return id;}

    /**
     * @brief setUrl 设置web的html的路径
     * @param url
     */
    void setUrl(QString url);
    /**
     * @brief setLocalFile 加载本地html文件
     * @param file
     */
    void setLocalFile(QString file);

    void callJavascript(const QString& js);

    void setFramelessWindow(bool frameless);

    void showWebInspector(bool show);

private:
    std::shared_ptr<QWebView> web;
    std::shared_ptr<QWebInspector> inspector;
    int id;
    void set_web_window_id();
private slots:
    void populateJavaScriptWindowsObject();
    void loadFinish(bool);
};

}


#endif // WEBUI_H
