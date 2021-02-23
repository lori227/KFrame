#include "KFHttpData.h"
#include "KFHttpClient.h"
#include "KFHttpsClient.h"

namespace KFrame
{
    KFHttpData::KFHttpData()
    {
        _type = 0;
        _http = nullptr;
        _function = nullptr;
    }

    KFHttpData::~KFHttpData()
    {
    }

    void KFHttpData::Request()
    {
        _result = _http->RunHttp( _type, _url, _data );
    }

    void KFHttpData::Response()
    {
        if ( _function != nullptr )
        {
            try
            {
                _function( _data, _result, _pass_back_data );
            }
            catch ( ... )
            {
                __LOG_ERROR__( "http response exception! result=[{}]", _result );
            }
        }
    }
}