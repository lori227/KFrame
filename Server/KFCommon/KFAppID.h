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
                uint32 _channel_id : 4;		// 16	渠道独立环境
                uint32 _zone_id : 12;		// 4095 小区
                uint32 _server_type : 8;	// 255	类型
                uint32 _worker_id : 8;		// 255	进程实例
            } _app_data;
        };

        KFAppID( uint32 appid )
        {
            _union._app_id = appid;
        }

        KFAppID( std::string strappid )
        {
            _union._app_data._channel_id = KFUtility::SplitValue< uint32 >( strappid, "." );
            _union._app_data._zone_id = KFUtility::SplitValue< uint32 >( strappid, "." );
            _union._app_data._server_type = KFUtility::SplitValue< uint32 >( strappid, "." );
            _union._app_data._worker_id = KFUtility::SplitValue< uint32 >( strappid, "." );
        }

        std::string ToString()
        {
            return __FORMAT__( "{}.{}.{}.{}", _union._app_data._channel_id, _union._app_data._zone_id,
                               _union._app_data._server_type, _union._app_data._worker_id );
        }


        static std::string ToString( uint32 appid )
        {
            KFAppID kfappid( appid );
            return kfappid.ToString();
        }

        static uint32 ToUInt32( std::string strappid )
        {
            KFAppID kfappid( strappid );
            return kfappid._union._app_id;
        }

    public:
        MyUnion _union;
    };


}

#endif // !__KF_APPID_H__
