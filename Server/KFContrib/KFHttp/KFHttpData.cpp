#include "KFHttpData.h"
#include "KFHttpClient.h"
#include "KFHttpsClient.h"

namespace KFrame
{
    KFHttpData::KFHttpData()
    {
        _http = nullptr;
        _function = nullptr;
    }

    KFHttpData::~KFHttpData()
    {
        __KF_DESTROY__( KFHttp, _http );
    }

    void KFHttpData::Request()
    {
        _result = _http->RunHttp( _url, _data );
    }

    void KFHttpData::Response()
    {
        if ( _function != nullptr )
        {
            try
            {
                _function( _data, _result );
            }
            catch ( ... )
            {
                __LOG_ERROR__( KFLogEnum::Logic, "http response exception! result=[{}]", _result );
            }
        }
    }
}