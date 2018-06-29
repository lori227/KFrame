#ifndef __KF_HTTP_DATA_H__
#define __KF_HTTP_DATA_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFHttp;
    class KFHttpClientManage;
    class KFHttpData
    {
    public:
        KFHttpData();
        ~KFHttpData();

        // 访问Http
        void StartMTHttp( KFHttpClientManage* clientmanage );

        // 执行逻辑
        void Run();
        ////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////
        // 访问结束
        void Finish();
    public:
        // 管理器
        KFHttpClientManage* _manage;

        // 访问地址
        std::string _url;

        // 发送数据
        std::string _data;

        // 接受数据
        std::string _result;

        // 回调函数
        KFHttpClientFunction _function;

        // httpclient
        KFHttp* _http;

    };
}


#endif