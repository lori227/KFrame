#ifndef __KF_APPID_H__
#define __KF_APPID_H__

#include "KFInclude.h"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
    class KFAppID
    {
    public:
        union MyUnion
        {
            uint64 _id;
            struct AppData
            {
                uint16 _channel_id;
                uint16 _server_type;
                uint16 _zone_id;
                uint16 _worker_id;
            } _app_data;
        };

        KFAppID()
        {
            _union._id = 0u;
        }

        KFAppID( uint64 appid )
        {
            _union._id = appid;
        }

        KFAppID( std::string strappid )
        {
            _union._app_data._channel_id = KFUtility::SplitValue< uint16 >( strappid, "." );
            _union._app_data._server_type = KFUtility::SplitValue< uint16 >( strappid, "." );
            _union._app_data._zone_id = KFUtility::SplitValue< uint16 >( strappid, "." );
            _union._app_data._worker_id = KFUtility::SplitValue< uint16 >( strappid, "." );
        }

        std::string ToString()
        {
            return __FORMAT__( "{}.{}.{}.{}", _union._app_data._channel_id, _union._app_data._zone_id,
                               _union._app_data._server_type, _union._app_data._worker_id );
        }


        static std::string ToString( uint64 appid )
        {
            KFAppID kfappid( appid );
            return kfappid.ToString();
        }

        static uint64 ToUInt64( std::string strappid )
        {
            KFAppID kfappid( strappid );
            return kfappid._union._id;
        }

    public:
        MyUnion _union;
    };


}

#endif // !__KF_APPID_H__
