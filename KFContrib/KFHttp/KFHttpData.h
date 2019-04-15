#ifndef __KF_HTTP_DATA_H__
#define __KF_HTTP_DATA_H__

#include "KFrame.h"

namespace KFrame
{
    class KFHttp;
    class KFHttpData
    {
    public:
        KFHttpData();
        ~KFHttpData();

        // 执行逻辑
        void Request();

        // 访问结束
        void Response();
    public:
        // 访问地址
        std::string _url;

        // 访问类型
        uint32 _type;

        // 发送数据
        std::string _data;

        // 回传数据
        std::string _args;

        // 接受数据
        std::string _result;

        // 回调函数
        KFHttpClientFunction _function;

        // httpclient
        KFHttp* _http;
    };
}


#endif