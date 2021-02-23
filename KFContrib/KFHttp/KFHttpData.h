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

        // send request to server
        void Request();

        // response and call back
        void Response();
    public:
        // request type
        uint32 _type;

        // server address
        std::string _url;

        // send data
        std::string _data;

        // pass back data
        std::string _pass_back_data;

        // recv data from server
        std::string _result;

        // call back function
        KFHttpClientFunction _function;

        // http logic data
        std::shared_ptr<KFHttp> _http;
    };
}

#endif