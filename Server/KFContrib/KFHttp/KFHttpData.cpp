#include "KFHttpData.h"
#include "KFHttpClient.h"
#include "KFHttpsClient.h"
#include "KFThread/KFThread.h"

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

    void KFHttpData::Finish()
    {
        if ( _function != nullptr )
        {
            _function( _data, _result );
        }
    }
}