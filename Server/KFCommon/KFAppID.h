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
            uint32 _app_id;
            struct AppData
            {
                uint8 _channel_id;
                uint8 _zone_id;
                uint8 _server_type;
                uint8 _instance_id;
            } _app_data;
        };

        KFAppID( uint32 appid )
        {
            _union._app_id = appid;
        }

        KFAppID( std::string strappid )
        {
            _union._app_data._channel_id = KFUtility::SplitValue< uint8 >( strappid, "." );
            _union._app_data._zone_id = KFUtility::SplitValue< uint8 >( strappid, "." );
            _union._app_data._server_type = KFUtility::SplitValue< uint8 >( strappid, "." );
            _union._app_data._instance_id = KFUtility::SplitValue< uint8 >( strappid, "." );
        }

        std::string ToString()
        {
            return __FORMAT__( "{}.{}.{}.{}", _union._app_data._channel_id, _union._app_data._zone_id,
                               _union._app_data._server_type, _union._app_data._instance_id );
        }


        static std::string ToString( uint32 appid )
        {
            KFAppID kfappid( appid );
            return kfappid.ToString();
        }

    public:
        MyUnion _union;
    };


}

#endif // !__KF_APPID_H__
