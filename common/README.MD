# common库
common库为pc/mac/ios/android提供基础的功能,主要完成网络请求,资源上传下载,图片处理等相关功能

#common库C++功能使用

## 日志相关命令
### 1.命令名 log
参数： 

    level : debug/info/warn/error  日志级别,使用四个值其一，默认为debug
    target : ui/net/io/lua/other 日志分类,使用其一，默认为other
    msg : 日志内容(必填)
    
回调：
  
    无
    

##http相关使用命令
###1.命令名 http
功能：进行简单的http请求
参数:

    type 请求种类 get/post,默认为get
    url 网址(必填)
    header 字典 header中设置值,可以为空
    form   字典  post请求时，表单值
    timeout 超时时间 默认为60s
    model asyn/sync,异步或者同步，默认为异步
    
回调：

    success : true/false      (函数调用成功或者失败)
    error   :  error_code     这个错误码定义在lua的error_code文件中
    reason  :  失败原因        错误原因的文字说明,主要是UI使用
    data    : {}              (函数返回的结果数据封装在这个字段中,如果请求结果是一非JSON对象，则在这个字段中再添加data字段)
    
    
###2.命令名 download
功能：下载文件，并有进度回调
参数：

    url 文件路径(必填)
    header 字典 header中设置值,可以为空
    timeout 超时
    process bool 是否回调进度，默认为false,不回调
    path 本地路径，包括文件名(必填)
    override bool 是否覆盖原有文件，不填写默认为true
    model asyn/sync,异步或者同步，默认为异步
    
回调：
   
    进度回调
    success : true/false      (函数调用成功或者失败)
    error   :  error_code     这个错误码定义在lua的error_code文件中
    reason  :  失败原因        错误原因的文字说明
    data {
        state  process表示是进度
        time   下载经过的秒数
        download  下载的字节数
        total   总字节数
    }
    
    下载结果回调
    success : true/false      (函数调用成功或者失败)
    error   :  error_code     这个错误码定义在lua的error_code文件中
    reason  :  失败原因        错误原因的文字说明
    data {
        state   finish  表示下载成功或者失败
        file_path   下载文件路径
        file_size   下载文件大小
    }
   
  
###3. 命令名 upload
功能：上传文件，并有上传进度回调
参数：
    
     url 文件路径
     header 字典 header中设置值,可以为空
     form 字典 form表单的值
     file_key   上传文件的key值，
     process true/false 是否回调进度，默认为false,不回调
     timeout 超时
     path 本地文件完整路径
     model asyn/sync,异步或者同步，默认为异步
回调：

    上传结果回调
    success : true/false      (函数调用成功或者失败)
    error   :  error_code     这个错误码定义在lua的error_code文件中
    reason  :  失败原因        错误原因的文字说明
    
    上传中进度回调
    data {
        state = "process";
        time = time; 上传时间
        upload = uploaded; 上传数据总数
        total = total; 文件大小总数
    }
    上传结束后回调
    data {
        upload_time = 10; 上传总时间
        upload_size = 1024; 上传大小
        data = "";       上传结束后返回的数据，没有的可以忽略 
        state = "finish";  标识上传完成
    }



## 算法相关命令
###1.命令名 zip
功能：对文件或者文件夹压缩；对zip压缩包解压
参数：

    action:  (compress/uncompress) 压缩或者解压，(必填)
    src_path :  需要操作的文件或者文件夹，如果是压缩，则为文件或者文件夹；如果是解压，则是一个zip包。(必填)
    dest_path :  操作之后的目标路径，如果是压缩，则是一个zip包；如果是解压，则为文件或者文件夹；(必填)
    code    :   密码，没有可以不填，选填
    
回调：
    
    success : true/false      (函数调用成功或者失败)
    error   :  error_code     这个错误码定义在lua的error_code文件中
    reason  :  失败原因        错误原因的文字说明
    data {
        dest_path   目标路径
    }
    
###2.命令名 md5
功能：对文件或者字符串计算md5值
参数：
    
    text    需要计算的字符串
    file    需要计算的文件路径，如文件不存在，则返回空字符串
    
回调：
    
    success : true/false      (函数调用成功或者失败)
    error   :  error_code     这个错误码定义在lua的error_code文件中
    reason  :  失败原因        错误原因的文字说明
    data {
        text_md5    字符串的md5值
        file_md5    文件的md5值
    }
    
###3.命令名 base64
功能：base64编码或者解码
参数：
    
     type    encode/decode   编码或者解码，默认为编码
     text    需要编码或者解码的字符串
     
回调：
     
    success : true/false      (函数调用成功或者失败)
    error   :  error_code     这个错误码定义在lua的error_code文件中
    reason  :  失败原因        错误原因的文字说明
    data {
         data    编码或者解码后的字符串
    }
     
     
###4.命令名 sha1
功能：计算sha1的值
参数：

    text     输入字符串
    file     输入的文件路径
回调：     

    success : true/false      (函数调用成功或者失败)
    error   :  error_code     这个错误码定义在lua的error_code文件中
    reason  :  失败原因        错误原因的文字说明
    data {
         text_sha1    计算字符串后的sha1值
         file_sha1    计算文件后的sha1值
    }


    
    
#lua使用规则
必须的lua文件有：

    bridge.lua  c++与lua交互的接口，定义了很多基本的函数功能。
    common.lua  引入需要使用的lua模块
    config.lua  配置接口
    table_check.lua     检测json数据是否符合给定的约定格式
    data_check_style.lua    记录json格式样式
    
最重要的bridge.lua中主要函数

lua中调用c++函数：

    --- lua调用c++函数的封装
    -- @param method c++中注册的函数名
    -- @param param 参数,为table值
    -- @param callback 回调,可为nil值
    function lua_c_bridge:call_native(method, param, callback)
    
输出日志：
    
    function lua_c_bridge:log_d(msg)
    function lua_c_bridge:log_i(msg)
    function lua_c_bridge:log_w(msg)
    function lua_c_bridge:log_e(msg)
    其中msg为输出日志内容
    
lua中将table等转换成字符串：

    --- 将lua中的table数据转换成json格式的字符串
    -- @param obj 数据
    -- @return 返回字符串表示
    function lua_c_bridge:tostring(obj)
    
在lua中注册供c++调用的函数
    
    --- 注册lua中可供c++调用的函数
    -- @param name 函数的命名空间,即函数的包装table名
    -- @param func 函数实体
    function lua_c_bridge:register_lua_function(name, func)

在lua中获取注册的lua函数，用于跨lua文件调用
    
    --- 得到在lua_c_bridge注册的函数
    -- @param name 函数名
    -- @return 返回注册的table值
    function lua_c_bridge:get_module(name)
    
    
        
    
##使用配置
配置参数主要是在config.lua中，通过config:get_config函数返回，目前支持的配置主要有：

    log_level 日志等级

##添加新的模块
如果需要添加一个新的lua模块,比如模块A,需要的步骤为：
    
    1.添加A.lua
    2.在common.lua中添加 require("A")
    3.在A.lua中添加一个table并注册，以至于c++可以调到这个函数。如
        require("bridge")    --引用bridge中函数
        local a_table = {}
        lua_c_bridge:register_lua_function("a_lua_func", a_table)
        其中a_lua_func为暴露给c++的函数名
    4.添加一个新函数
        function a_table:get(param, callback)
        end
    其中callback的参数有6个，参见bridge中callback函数。编写好get函数后，在c++调用get函数的规则为：
    a_lua_func.get
    
    lua中调用c++函数的方法为调用function lua_c_bridge:call_native(method, param, callback)



#添加新的c++模块规则
添加新的c++模块，需要在init文件目录下添加一个##_init类，其中##为你的功能。在这个类中实现一个静态的init函数，参见http_init类。
。这个init方法中使用cmd_factory注册对外使用的函数。