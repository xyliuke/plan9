#include "biz_proxy.h"
#include <init/common.h>
#include <log/log_wrap.h>

namespace plan9_ui {

biz_proxy::biz_proxy()
{

}

void biz_proxy::init(const QString &path, const QString &lua)
{
    plan9::common::init("../../../", "../../../lua");
}

}


