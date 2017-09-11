#ifndef BIZ_PROXY_H
#define BIZ_PROXY_H

#include <QObject>

namespace plan9_ui {
class biz_proxy
{
public:
    biz_proxy();

    static void init(const QString& path, const QString& lua);

};
}

#endif // BIZ_PROXY_H
